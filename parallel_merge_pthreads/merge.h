#include <stdio.h>
#include <pthread.h>

#ifndef PARALLEL_MEGRE_PTHREADS_MERGE_H
#define PARALLEL_MEGRE_PTHREADS_MERGE_H

typedef struct chunk_t {
    int * array;
    size_t arr_len;
    size_t chunk_len;
    size_t index;
} chunk_t;

typedef struct merge_iteration_t {
    int * array;
    int * result;
    size_t arr_len;
    size_t i, j;
} merge_iteration_t;

int comparator (const void * a, const void * b);
void gen_array(int * a, size_t len);
void print_array(int * a, size_t len, FILE * out);
void insertion_sort(int * a, size_t len);

void * sort_chunk(void * args);
void sort_chunks(int * array, size_t arr_len, size_t chunk_len,
                 pthread_t * threads, size_t thread_count);

size_t max (size_t a, size_t b); //used only for size_t variables
size_t bin_search(int x, const int * a, size_t left, size_t right);

void swap(size_t * a, size_t * b); //used only for size_t variables
void simple_merge(const int * array,
                  size_t left_1, size_t right_1,
                  size_t left_2, size_t right_2,
                  int * result, size_t left_3);

void merge(int * array,
           size_t left_1, size_t right_1,
           size_t left_2, size_t right_2,
           int * result, size_t left_3);
void * merge_iteration(void * args);

void parallel_merge_sort (int * array, size_t arr_len,
                          size_t chunk_len, int * result,
                          pthread_t * threads, size_t thread_count);

#endif //PARALLEL_MEGRE_PTHREADS_MERGE_H
