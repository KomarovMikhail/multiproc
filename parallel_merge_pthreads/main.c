#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <omp.h>
#include <memory.h>

typedef struct chunk_t {
    int * array;
    size_t arr_len;
    size_t chunk_len;
    size_t index;
} chunk_t;

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

void insertion_sort(int * a, size_t len)
{
    for (size_t i = 1; i < len; i++)
    {
        int x = a[i];
        size_t j = i;
        while (j > 0 && a[j-1] > x)
        {
            a[j] = a[j - 1];
            j = j - 1;
        }
        a[j] = x;
    }

}

void * sort_chunk(void * args)
{
    struct chunk_t * chunk_params = args;

    if (chunk_params->index + chunk_params->chunk_len <= chunk_params->arr_len)
    {
        //insertion_sort(array + i, chunk_len);
        qsort(chunk_params->array + chunk_params->index,
              chunk_params->chunk_len, sizeof(int), comparator);
    }
    else
    {
        //insertion_sort(array + i, arr_len - i);
        qsort(chunk_params->array + chunk_params->index,
              chunk_params->arr_len - chunk_params->index, sizeof(int), comparator);
    }
}

void sort_chunks(int * array, const size_t arr_len, const size_t chunk_len,
                 pthread_t * threads, const size_t thread_count)
{
    size_t i = 0;
    struct chunk_t chunk_params;
    chunk_params.array = array;
    chunk_params.arr_len = arr_len;
    chunk_params.chunk_len = chunk_len;

    while (i < arr_len)
    {
        for (size_t j = 0; j < thread_count; j++)
        {
            chunk_params.index = i;
            pthread_create(threads + j, NULL, sort_chunk, &chunk_params);
            pthread_join(threads[j], NULL);

            i += chunk_len;
            if (i >= arr_len)
            {
                break;
            }
        }
    }
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

void simple_merge(const int * array,
                  size_t left_1, size_t right_1,
                  size_t left_2, size_t right_2,
                  int * result, size_t left_3)
{
    size_t index_1 = left_1;
    size_t index_2 = left_2;
    size_t index_3 = left_3;

    size_t n_1 = right_1 - left_1 + 1;
    size_t n_2 = right_2 - left_2 + 1;

    if (n_1 == 0 && n_2 == 0)
    {
        return;
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
           int * result, size_t left_3)
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


        {
            simple_merge(array, left_1, mid_1 - 1, left_2, mid_2 - 1, result, left_3);
        }

        {
            simple_merge(array, mid_1 + 1, right_1, mid_2, right_2, result, mid_3 + 1);
        }
    }
}

void parallel_merge_sort (int * array, size_t arr_len,
                          size_t chunk_len, int * result,
                          pthread_t * threads, size_t thread_count)
{
    sort_chunks(array, arr_len, chunk_len, threads, thread_count);

    int flag = 0;

    for(size_t j = 2 * chunk_len; j <= 2 * arr_len; j += j)
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

        for (size_t i = 0; i < arr_len; i += j)
        {
            if (i + j <= arr_len)
            {
                merge(array, i, i + j / 2 - 1, i + j / 2, i + j - 1, result, i);
            }
            else
            {
                if (i + j / 2 < arr_len)
                {
                    merge(array, i, i + j / 2 - 1, i + j / 2, arr_len - 1, result, i);
                }
            }
        }
    }
}



int main(int argc, char ** argv)
{
    int * arr;
    int * result;
    int * arr_copy;
    size_t n, m, thread_count;
    FILE * stats;
    FILE * data;
    double start_time, end_time_1, end_time_2;
    pthread_t * threads;

    if (argc != 4)
    {
        printf("Wrong number of args\n");
        exit(1);
    }
    else
    {
        //initialization
        n = (unsigned) atoi(argv[1]);
        m = (unsigned) atoi(argv[2]);
        thread_count = (unsigned) atoi(argv[3]);
        arr = (int *)malloc(sizeof(int) * n);
        result = (int *)malloc((sizeof(int) * n));
        arr_copy = (int *)malloc((sizeof(int) * n));
        threads = (pthread_t *)malloc((sizeof(pthread_t) * thread_count));
        stats = fopen("stats.txt", "w");
        data = fopen("data.txt", "w");

        if (arr == NULL || result == NULL || arr_copy == NULL || threads == NULL)
        {
            printf("virtual memory exhausted\n");
            exit(1);
        }

        srand((unsigned int)time(NULL));
    }

    // preparation for sorting
    gen_array(arr, n);
    memcpy(arr_copy, arr, sizeof(int) * n);
    fprintf(data, "Original array:\n");
    print_array(arr, n, stdout);

    //parallel merge sort
    start_time = omp_get_wtime();
    parallel_merge_sort(arr, n, m, result, threads, thread_count);
    end_time_1 = omp_get_wtime() - start_time;

    //quick sort
    start_time = omp_get_wtime();
    qsort(arr_copy, n, sizeof(int), comparator);
    end_time_2 = omp_get_wtime() - start_time;

    printf("Parallel merge sort time: %lf\nQuick sort time: %lf\n", end_time_1, end_time_2);

    fprintf(data, "Sorted array:\n");
    print_array(result, n, stdout);
    fprintf(stats, "%lfs %ld %ld %ld", end_time_1, n, m, thread_count);

    fclose(stats);
    fclose(data);
    free(arr);
    free(result);
    free(arr_copy);
    free(threads);

    return 0;
}