#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <memory.h>
#include <omp.h>
#include "merge.h"



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