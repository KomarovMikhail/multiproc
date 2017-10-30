#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define ACCURACY 1000
#define LEFT 1
#define RIGHT 2
#define UP 3
#define DOWN 4

typedef struct point_t {
	size_t x, y, area_id;
} point_t;

void gen_points(point_t * points, size_t point_count, size_t l, size_t a, size_t b)
{
	for (size_t i = 0; i < b; i++)
	{
		for (size_t j = 0; j < a; j++)
		{
			for (size_t k = 0; k < point_count; k++)
			{
				point_t point;
				point.x = (size_t) rand() % l;
				point.y = (size_t) rand() % l;
				point.area_id = i + j;
				points[i+j] = point;
			}
		}
	}
	return;
}

int get_direction(int rand_int, double p_left, double p_right, double p_up, double p_down)
{
	int left_border = p_left * ACCURACY;
	int right_border = (p_right + p_left) * ACCURACY;
	int up_border = (p_up + p_right + p_left) * ACCURACY;
	int down_border = (p_down + p_up + p_right + p_left) * ACCURACY;
	
	if (rand_int < left_border)
	{
		return LEFT;
	}
	if (rand_int < right_border)
	{
		return RIGHT;
	}
	if (rand_int < up_border)
	{
		return UP;
	}
	if (rand_int < down_border)
	{
		return DOWN;
	}
}

int main(int argc, char ** argv) {

    size_t l, a, b, steps, point_count;
    double start_time, time, p_left, p_right, p_up, p_down;
    int rank, size;

    if (argc != 10)
    {
        printf("Wrong number of args\n");
        return 1;
    }
    else
    {
        l = (size_t) atoi(argv[1]);
        a = (size_t) atoi(argv[2]);
        b = (size_t) atoi(argv[3]);
        steps = (size_t) atoi(argv[4]);
        point_count = (size_t) atoi(argv[5]);
        p_left = atof(argv[6]);
        p_right = atof(argv[7]);
        p_up = atof(argv[8]);
        p_down = atof(argv[9]);
        srand((unsigned int)time(NULL));
    }
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	




    FILE * out = fopen("stats.txt", "w");
    if (out == NULL)
    {
        printf("Can't open the file\n");
        exit(1);
    }

    fclose(out);
    
    MPI_Finalize();

    printf("total time: %lf\n", time);

    return 0;
}




