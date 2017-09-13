#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

#define ACCURACY 1000

int main(int argc, char ** argv) {
    int a, b, x, n, thread_num;
    double p, b_probability, start_time, time;
    int b_count = 0;
    double steps_mean = 0;
    omp_lock_t lock;

    if (argc != 7)
    {
        printf("Wrong number of args\n");
        return 0;
    }
    else
    {
        a = atoi(argv[1]);
        b = atoi(argv[2]);
        x = atoi(argv[3]);
        n = atoi(argv[4]);
        p = atof(argv[5]);
        thread_num = atoi(argv[6]);
    }

    start_time = omp_get_wtime();
    omp_init_lock(&lock);
    omp_set_num_threads(thread_num);
    #pragma omp parallel for
    for(unsigned int i = 0; i < n; i++)
    {
        int curr_pos = x;
        unsigned int seed = (unsigned int) clock();
        unsigned int next_step;
        unsigned int steps_done = 0;

        while (curr_pos != b && curr_pos != a)
        {
            next_step = (unsigned) rand_r(&seed) % ACCURACY < ACCURACY * p ? 0 : 1;
            if (next_step == 0) {
                curr_pos++;
            } else {
                curr_pos--;
            }
            steps_done++;
        }

        if (curr_pos == b)
        {
            omp_set_lock(&lock);
            b_count++;
            steps_mean += steps_done;
            omp_unset_lock(&lock);
        }

    }

    time = omp_get_wtime() - start_time;

    b_probability = (double) b_count / (double) n;
    steps_mean /= (double) n;

    FILE * out = fopen("stats.txt", "w");
    if (out == NULL)
    {
        printf("Can't open the file\n");
        exit(1);
    }
    fprintf(out, "%.2f %.1lf %lfs %d %d %d %d %.2f %d\n", b_probability, steps_mean, time, a, b, x, n, p, thread_num);
    fclose(out);

    //printf("total time: %lf\n", time);

    return 0;
}