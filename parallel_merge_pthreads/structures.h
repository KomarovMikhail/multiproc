#ifndef PARALLEL_MEGRE_PTHREADS_STRUCTURES_H
#define PARALLEL_MEGRE_PTHREADS_STRUCTURES_H

#include <pthread.h>
//subsidiary structures

typedef struct chunk_t {
    int * array;
    size_t arr_len, chunk_len, index;
} chunk_t;

typedef struct simple_merge_t {
    int * array, *result;
    size_t left_1, right_1, left_2, right_2, left_3;
} simple_merge_t;

typedef struct merge_iteration_t {
    int * array, * result;
    size_t arr_len;
    size_t i, j, thread_id, thread_count;
    pthread_t * threads;
    int parallel_flag; //1 if thread_count > 1, 0 else
    simple_merge_t * arg_params;
} merge_iteration_t;

simple_merge_t simple_merge_init(int * array, int * result,
                                 size_t left_1, size_t right_1,
                                 size_t left_2, size_t right_2, size_t left_3);
merge_iteration_t merge_iteration_init(int * array, int * result,
                                       size_t arr_len, size_t i, size_t j,
                                       size_t thread_id, pthread_t * threads,
                                       int flag, size_t thread_count, simple_merge_t * arg_params);
chunk_t chunk_init(int * array, size_t arr_len, size_t chunk_len, size_t index);

#endif //PARALLEL_MEGRE_PTHREADS_STRUCTURES_H
