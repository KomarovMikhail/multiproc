#include "structures.h"

chunk_t chunk_init(int * array, size_t arr_len, size_t chunk_len, size_t index)
{
    chunk_t res;
    res.array = array;
    res.arr_len = arr_len;
    res.chunk_len = chunk_len;
    res.index = index;
    return res;
}

merge_iteration_t merge_iteration_init(int * array, int * result,
                                       size_t arr_len, size_t i, size_t j,
                                       size_t thread_id, pthread_t * threads,
                                       int flag, size_t thread_count, simple_merge_t * arg_params)
{
    merge_iteration_t res;
    res.array = array;
    res.result = result;
    res.arr_len = arr_len;
    res.i = i;
    res.j = j;
    res.thread_id = thread_id;
    res.threads = threads;
    res.parallel_flag = flag;
    res.thread_count = thread_count;
    res.arg_params = arg_params;
    return res;
}

simple_merge_t simple_merge_init(int * array, int * result,
                                 size_t left_1, size_t right_1,
                                 size_t left_2, size_t right_2, size_t left_3)
{
    simple_merge_t res;
    res.array = array;
    res.result = result;
    res.left_1 = left_1;
    res.left_2 = left_2;
    res.left_3 = left_3;
    res.right_1= right_1;
    res.right_2 = right_2;
    return res;
}
