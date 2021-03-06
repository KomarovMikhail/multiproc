#include "merge.h"
#include <stdlib.h>
#include <memory.h>

int comparator (const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}

void gen_array(int * a, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        a[i] = (int) (rand() % len);
    }
}

void print_array(int * a, size_t len, FILE * out)
{
    for (size_t i = 0; i < len; i++)
    {
        fprintf(out, "%d ", a[i]);
    }
    fprintf(out, "\n");
}

void * sort_chunk(void * args)
{
    struct chunk_t * chunk_params = args;

    if (chunk_params->index + chunk_params->chunk_len <= chunk_params->arr_len)
    {
        qsort(chunk_params->array + chunk_params->index,
              chunk_params->chunk_len, sizeof(int), comparator);
    }
    else
    {
        qsort(chunk_params->array + chunk_params->index,
              chunk_params->arr_len - chunk_params->index, sizeof(int), comparator);
    }
}

void sort_chunks(int * array, const size_t arr_len, const size_t chunk_len,
                 pthread_t * threads, const size_t thread_count)
{
    size_t i = 0;

    chunk_t * chunk_params = (chunk_t *) malloc (sizeof(chunk_t) * thread_count);

    while (i < arr_len)
    {
        size_t threads_working = 0;
        for (size_t j = 0; j < thread_count; j++)
        {
            chunk_params[j] = chunk_init(array, arr_len, chunk_len, i);
            pthread_create(threads + j, NULL, sort_chunk, chunk_params + j);
            threads_working++;

            i += chunk_len;
            if (i >= arr_len)
            {
                break;
            }
        }
        for (size_t t = 0; t < threads_working; t++)
        {
            pthread_join(threads[t], NULL);
        }
    }
    free(chunk_params);
}

size_t max (size_t a, size_t b) //used only for size_t variables
{
    return a > b ? a : b;
}

size_t bin_search(int x, const int * a, size_t left, size_t right)
{
    size_t low = left;
    size_t high = max(left, right + 1);
    while (low < high)
    {
        size_t mid = (low + high) / 2;
        if (x <= a[mid])
        {
            high = mid;
        }
        else
        {
            low = mid + 1;
        }
    }
    return high;
}

void swap(size_t * a, size_t * b) //used only for size_t variables
{
    * a ^= * b;
    * b ^= * a;
    * a ^= * b;
}

void * simple_merge(void * args)
{
    simple_merge_t * params = (simple_merge_t *) args;
    int * array = params->array;
    int *result = params->result;
    size_t left_1 = params->left_1;
    size_t left_2 = params->left_2;
    size_t left_3 = params->left_3;
    size_t right_1 = params->right_1;
    size_t right_2 = params->right_2;

    size_t index_1 = left_1;
    size_t index_2 = left_2;
    size_t index_3 = left_3;

    size_t n_1 = right_1 - left_1 + 1;
    size_t n_2 = right_2 - left_2 + 1;

    if (n_1 == 0 && n_2 == 0)
    {
        return NULL;
    }
    else
    {
        while (index_1 <= right_1 && index_2 <= right_2)
        {
            if (array[index_1] <= array[index_2])
            {
                result[index_3++] = array[index_1++];
            }
            else
            {
                result[index_3++] = array[index_2++];
            }
        }

        while (index_1 <= right_1)
        {
            result[index_3++] = array[index_1++];
        }

        while (index_2 <= right_2)
        {
            result[index_3++] = array[index_2++];
        }
    }
}

void merge(int * array,
           size_t left_1, size_t right_1,
           size_t left_2, size_t right_2,
           int * result, size_t left_3,
           pthread_t * threads, size_t thread_id,
           int flag, simple_merge_t * arg_params, size_t thread_count)
{
    size_t n_1 = right_1 - left_1 + 1;
    size_t n_2 = right_2 - left_2 + 1;

    if (n_1 < n_2)
    {
        swap(&left_1, &left_2);
        swap(&right_1, &right_2);
        swap(&n_1, &n_2);
    }
    if (n_1 == 0)
    {
        return;
    }
    else
    {
        size_t mid_1 = (right_1 + left_1) / 2;
        size_t mid_2 = bin_search(array[mid_1], array, left_2, right_2);
        size_t mid_3 = left_3 + (mid_1 - left_1) + (mid_2 - left_2);
        result[mid_3] = array[mid_1];

        if (flag)
        {
            arg_params[thread_id] = simple_merge_init(array, result, left_1, mid_1 - 1,
                                                      left_2, mid_2 - 1, left_3);
            arg_params[thread_id + thread_count/2] = simple_merge_init(array, result, mid_1 + 1, right_1,
                                                      mid_2, right_2, mid_3 + 1);


            pthread_create(threads + thread_id + thread_count/2, NULL,
                           simple_merge, arg_params + thread_id + thread_count/2);
            simple_merge(arg_params + thread_id);
            pthread_join(threads[thread_id + thread_count/2], NULL);
        }
        else
        {
            simple_merge_t args_1 = simple_merge_init(array, result, left_1, mid_1 - 1,
                                                      left_2, mid_2 - 1, left_3);
            simple_merge_t args_2 = simple_merge_init(array, result, mid_1 + 1, right_1,
                                                      mid_2, right_2, mid_3 + 1);
            simple_merge(&args_1);
            simple_merge(&args_2);
        }
    }
}

void * merge_iteration(void * args)
{
    struct merge_iteration_t * params = args;
    size_t i = params->i;
    size_t j = params->j;
    size_t arr_len = params->arr_len;
    size_t thread_id = params->thread_id;
    int * array = params->array;
    int * result = params->result;
    pthread_t * threads = params->threads;
    int flag = params->parallel_flag;
    simple_merge_t * arg_params = params->arg_params;
    size_t thread_count = params->thread_count;


    if (i + j <= arr_len)
    {
        merge(array, i, i + j / 2 - 1, i + j / 2, i + j - 1, result, i,
              threads, thread_id, flag, arg_params, thread_count);
    }
    else
    {
        if (i + j / 2 < arr_len)
        {
            merge(array, i, i + j / 2 - 1, i + j / 2, arr_len - 1, result, i,
                  threads, thread_id, flag, arg_params, thread_count);
        }

    }

}

void parallel_merge_sort (int * array, size_t arr_len,
                          size_t chunk_len, int * result,
                          pthread_t * threads, size_t thread_count)
{
    sort_chunks(array, arr_len, chunk_len, threads, thread_count);

    int flag = 0;

    if (thread_count == 1)
    {
        merge_iteration_t * params = (merge_iteration_t *)malloc(sizeof(merge_iteration_t));

        for (size_t j = 2 * chunk_len; j <= 2 * arr_len; j += j)
        {
            if (flag)
            {
                memcpy(array, result, sizeof(int) * arr_len);
            }
            else
            {
                memcpy(result, array, sizeof(int) * arr_len);
                flag = 1;
            }

            size_t i;

            for (i = 0; i < arr_len; i += j)
            {
                params[0] = merge_iteration_init(array, result, arr_len, i, j,
                                                 0, threads, 0, 1, NULL);
                merge_iteration(params);
            }
        }
        free(params);
    }
    else
    {
        merge_iteration_t * params = (merge_iteration_t *) malloc(sizeof(merge_iteration_t) *
                                                                 thread_count / 2);
        simple_merge_t * arg_params = (simple_merge_t *) malloc(sizeof(simple_merge_t) *
                                                                 thread_count);
        for (size_t j = 2 * chunk_len; j <= 2 * arr_len; j += j)
        {
            if (flag)
            {
                memcpy(array, result, sizeof(int) * arr_len);
            }
            else
            {
                memcpy(result, array, sizeof(int) * arr_len);
                flag = 1;
            }

            size_t i = 0;

            while (i < arr_len)
            {
                size_t threads_working = 0;
                for (size_t t = 0; t < thread_count / 2; t++)
                {
                    params[t] = merge_iteration_init(array, result, arr_len, i, j,
                                                     t, threads, 1, thread_count, arg_params);
                    pthread_create(threads + t, NULL, merge_iteration, params + t);
                    threads_working++;

                    i += j;
                    if (i >= arr_len)
                    {
                        break;
                    }
                }
                for (size_t t = 0; t < threads_working; t++)
                {
                    pthread_join(threads[t], NULL);
                }

            }
        }
        free(params);
        free(arg_params);
    }
}
