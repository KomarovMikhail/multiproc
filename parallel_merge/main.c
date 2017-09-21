#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>

//int comparator (const void * a, const void * b)
//{
//    return ( *(int*)a - *(int*)b );
//}

void gen_array(int * a, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        a[i] = (int) (rand() % len);
    }
}

void print_array(int * a, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        printf("%d ", a[i]);
    }
    printf("\n");
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

void sort_chunks(int * array, size_t arr_len, size_t chunk_len)
{
    #pragma omp parallel for
    for (int i = 0; i < arr_len; i+= chunk_len)
    {
        if (i + chunk_len <= arr_len)
        {
            insertion_sort(array + i, chunk_len);
        }
        else
        {
            insertion_sort(array + i, arr_len - i);
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

        #pragma omp parallel
        {
            #pragma omp sections
            {
                #pragma omp section
                {
                    merge(array, left_1, mid_1 - 1, left_2, mid_2 - 1, result, left_3);
                }
                #pragma omp section
                {
                    merge(array, mid_1 + 1, right_1, mid_2, right_2, result, mid_3 + 1);
                }
            }
        }
    }
}

void parallel_merge_sort (int * array, size_t arr_len,
                     size_t chunk_len, int * result)
{
    int flag = 0;
    for(size_t j = 2 * chunk_len; j <= 2 * arr_len; j *= 2)
    {
        if (flag)
        {
            memcpy(array, result, sizeof(int) * arr_len);
        }
        else
        {
            memcpy(result, array, sizeof(int) * arr_len);
        }

        #pragma omp parallel for
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
        flag = 1;

    }
}



int main(int argc, char ** argv)
{
    int * arr;
    size_t n, m;
    int thread_count;

    if (argc != 4)
    {
        printf("Wrong number of args\n");
        exit(1);
    }
    else
    {
        n = (unsigned) atoi(argv[1]);
        m = (unsigned) atoi(argv[2]);
        thread_count = atoi(argv[3]);
        arr = (int *)malloc(sizeof(int) * n);

        if (arr == NULL)
        {
            printf("virtual memory exhausted\n");
            exit(1);
        }

        srand((unsigned int)time(NULL));
    }

    print_array(arr, n);

    // preparation for merging
    gen_array(arr, n);
    omp_set_num_threads(thread_count);
    sort_chunks(arr, n, m);
    int * result = (int *)malloc((sizeof(int) * n));

    //merging
    parallel_merge_sort(arr, n, m, result);

    print_array(result, n);

    free(arr);
    free(result);

    return 0;
}