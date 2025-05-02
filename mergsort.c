#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define arraysize 10

int unsorted_array[arraysize];
int sorted_array[arraysize];

void insertion_sort(int list[], int start, int end) {
    int j, temp;
    for (int i = start + 1 ; i < end ; i++) {
        j = i - 1;
        temp = list[i];
        while (list[j] > temp && j >= start) {
            list[j + 1] = list[j];
            j--;
        }
        list[j + 1] = temp;
    }
}

void merge(int a[], int left[], int right[], int left_size, int right_size) {
    int i = 0, j = 0, Final = 0;
    while (i < left_size && j < right_size) {
        if (left[i] <= right[j]) {
            a[Final++] = left[i++];
        } else {
            a[Final++] = right[j++];
        }
    }
    while (i < left_size) a[Final++] = left[i++];
    while (j < right_size) a[Final++] = right[j++];
}

void* sort_thread(void* arg) {
    int* idcs = (int*)arg;
    int start = idcs[0];    //aval bakhshi az araye ke mikhad sort beshe
    int end = idcs[1];      //entehaye bakhshi az araye ke mikhad sort beshe
    insertion_sort(unsorted_array, start, end);
    pthread_exit(NULL);
}

void* merge_thread(void* arg) {
    int mid = arraysize / 2;
    int left[mid];
    int right[arraysize - mid];

    for (int i = 0; i < mid; i++){
       left[i] = unsorted_array[i];}

    for (int i = mid; i < arraysize; i++){
        right[i - mid] = unsorted_array[i];}

    merge(sorted_array, left, right, mid, arraysize - mid);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[3];
    int idcs1[2] = {0, arraysize / 2};
    int idcs2[2] = {arraysize / 2, arraysize};

    printf("Enter %d integers to sort:\n", arraysize);
    for (int i = 0; i < arraysize; i++) {
        scanf("%d", &unsorted_array[i]);
    }

    pthread_create(&threads[0], NULL, sort_thread, idcs1);
    pthread_create(&threads[1], NULL, sort_thread, idcs2);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_create(&threads[2], NULL, merge_thread, NULL);
    pthread_join(threads[2], NULL);

    printf("final result:\n");
    for (int i = 0; i < arraysize; i++) {
        printf("%d", sorted_array[i]);
        if(i!=arraysize-1){
        printf(",");}
    }
    printf("\n");

    return 0;
}