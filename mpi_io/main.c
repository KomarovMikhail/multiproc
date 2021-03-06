#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

typedef struct point_t {
    int x, y, r;
} point_t;

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int l, a, b, N, curr_proc_x, curr_proc_y, mask_size, rank, size, seed, offset;
    double start_time, end_time, full_time;
    int *seeds, *mask;
    struct point_t *points;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 5)
    {
        printf("Wrong number of args\n");
        MPI_Finalize();
        return 1;
    }
    else
    {
	/* initialization  */
        l = atoi(argv[1]);
        a = atoi(argv[2]);
        b = atoi(argv[3]);
        N = atoi(argv[4]);

	offset = ((rank % a) * l + (rank / a) * a * l * l) * size;

        points = (struct point_t*)malloc(N * sizeof(struct point_t));
        seeds = (int*)malloc(size * sizeof(int));

	
        if (points == NULL || seeds == NULL)
        {
            printf("virtual memory exhausted\n");
            MPI_Finalize();
            return 1;
        }
    }

    start_time = MPI_Wtime();

    if (size != a * b)
    {
        printf("Wrong args\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        srand(time(NULL));

        for (int i = 0; i < size; ++i)
        {
            seeds[i] = rand();
        }
    }

    MPI_Scatter(seeds, 1, MPI_UNSIGNED, &seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    free(seeds);

    for (int i = 0; i < N; ++i)
    {
	struct point_t new_point;
	new_point.x = rand_r((unsigned int*)&seed) % l;
        new_point.y = rand_r((unsigned int*)&seed) % l;
        new_point.r = rand_r((unsigned int*)&seed) % (a * b);
	points[i] = new_point;
    }

    mask = (int*)malloc(l * l * size * sizeof(int));

    if (mask == NULL)
    {
        printf("virtual memory exhausted\n");
        MPI_Finalize();
        return 1;
    }

    for (int i = 0; i < l * l * size; ++i)
    {
        mask[i] = 0;
    }

    MPI_File bin_file;
    MPI_File_delete("data.bin", MPI_INFO_NULL);
    MPI_File_open(MPI_COMM_WORLD, "data.bin", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &bin_file);

    for (int i = 0; i < N; ++i)
    {
        mask[points[i].y * l * size + points[i].x * size + points[i].r] += 1;
    }

    MPI_Aint intex;
    MPI_Aint e;
    MPI_Type_get_extent(MPI_INT, &e, &intex);

    MPI_Datatype view;
    MPI_Type_vector(l, l * size, l * a * size, MPI_INT, &view);
    MPI_Type_commit(&view);

    MPI_File_set_view(bin_file, offset * sizeof(int), MPI_INT, view, "native", MPI_INFO_NULL);

    MPI_File_write(bin_file, mask, l * l * size, MPI_INT, MPI_STATUS_IGNORE);
    MPI_Type_free(&view);

    MPI_File_close(&bin_file);

    free(mask);

    end_time = MPI_Wtime();

    if (rank == 0)
    {
        full_time = end_time - start_time;
        FILE * stats = fopen("stats.txt", "w");

        if (stats == NULL)
        {
            printf("Cannot open the file\n");
            MPI_Finalize();
            return 1;
        }

        fprintf(stats, "%d %d %d %d %fs\n", l, a, b, N, full_time);

        fclose(stats);
    }

    free(points);
    MPI_Finalize();

    return 0;
}

