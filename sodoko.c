#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define GRID_SIZE 9
#define SUBGRID_SIZE 3
#define NUM_THREADS 11

int grid[GRID_SIZE][GRID_SIZE];
int valid[NUM_THREADS] = {0};

// Parameter struct for subgrid threads
typedef struct {
    int row;
    int col;
    int idx;
} parameters;

// Thread function to check all rows
void* checkRows(void* args) {
    for (int i = 0; i < GRID_SIZE; i++) {
        int seen[GRID_SIZE + 1] = {0};
        for (int j = 0; j < GRID_SIZE; j++) {
            int num = grid[i][j];
            if (num < 1 || num > GRID_SIZE || seen[num]) {
                valid[0] = 0;
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    valid[0] = 1;
    pthread_exit(NULL);
}

// Thread function to check all columns
void* checkColumns(void* args) {
    for (int j = 0; j < GRID_SIZE; j++) {
        int seen[GRID_SIZE + 1] = {0};
        for (int i = 0; i < GRID_SIZE; i++) {
            int num = grid[i][j];
            if (num < 1 || num > GRID_SIZE || seen[num]) {
                valid[1] = 0;
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    valid[1] = 1;
    pthread_exit(NULL);
}

// Thread function to check a 3x3 subgrid
void* checkSubgrid(void* args) {
    parameters* p = (parameters*) args;
    int seen[GRID_SIZE + 1] = {0};
    for (int i = p->row; i < p->row + SUBGRID_SIZE; i++) {
        for (int j = p->col; j < p->col + SUBGRID_SIZE; j++) {
            int num = grid[i][j];
            if (num < 1 || num > GRID_SIZE || seen[num]) {
                valid[p->idx] = 0;
                free(p);
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    valid[p->idx] = 1;
    free(p);
    pthread_exit(NULL);
}

int main() {
    // Read the Sudoku grid from standard input
    printf("Enter Sudoku grid (9 rows of 9 numbers each):\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (scanf("%d", &grid[i][j]) != 1) {
                fprintf(stderr, "Invalid input\n");
                return EXIT_FAILURE;
            }
        }
    }

    pthread_t threads[NUM_THREADS];
    int rc;

    // Create thread to check rows
    rc = pthread_create(&threads[0], NULL, checkRows, NULL);
    if (rc) {
        fprintf(stderr, "Error creating row thread: %d\n", rc);
        return EXIT_FAILURE;
    }

    // Create thread to check columns
    rc = pthread_create(&threads[1], NULL, checkColumns, NULL);
    if (rc) {
        fprintf(stderr, "Error creating column thread: %d\n", rc);
        return EXIT_FAILURE;
    }

    // Create 9 threads to check each 3x3 subgrid
    for (int i = 0; i < GRID_SIZE; i++) {
        parameters* data = malloc(sizeof(parameters));
        data->row = (i / SUBGRID_SIZE) * SUBGRID_SIZE;
        data->col = (i % SUBGRID_SIZE) * SUBGRID_SIZE;
        data->idx = i + 2;  // threads[2] to threads[10]
        rc = pthread_create(&threads[i + 2], NULL, checkSubgrid, data);
        if (rc) {
            fprintf(stderr, "Error creating subgrid thread %d: %d\n", i, rc);
            free(data);
            return EXIT_FAILURE;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Check results
    for (int i = 0; i < NUM_THREADS; i++) {
        if (valid[i] == 0) {
            printf("Sudoku solution is invalid.\n");
            return EXIT_SUCCESS;
        }
    }

    printf("Sudoku solution is valid!\n");
    return EXIT_SUCCESS;
}