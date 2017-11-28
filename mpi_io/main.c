#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

typedef struct {
    size_t x, y, r;
} point_t;

int main(int argc, char * argv[]) {
    MPI_Init(&argc, &argv);

    size_t l, a, b, N, curr_proc_x, curr_proc_y, mask_size;
    int rank, size, seed, offset;
    double start_time, end_time, full_time;
    int *seeds, *mask;
    point_t *points;

    if (argc != 5)
    {
        printf("Wrong number of args\n");
        MPI_Finalize();
        return 1;
    }
    else
    {
        l = (unsigned) atoi(argv[1]);
        a = (unsigned) atoi(argv[2]);
        b = (unsigned) atoi(argv[3]);
        N = (unsigned) atoi(argv[4]);

        points = (point_t*)malloc(N * sizeof(point_t));
        seeds = (int*)malloc(a * b * sizeof(int));

        if (points == NULL || seeds == NULL)
        {
            printf("virtual memory exhausted\n");
            MPI_Finalize();
            return 1;
        }

        curr_proc_x = rank % a;
        curr_proc_y = rank / a;
    }

    start_time = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != a * b)
    {
        printf("Wrong args\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0)
    {
        srand((unsigned)time(NULL));

        for (int i = 0; i < size; ++i)
        {
            seeds[i] = rand();
        }
    }

    MPI_Scatter(seeds, 1, MPI_UNSIGNED, &seed, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    free(seeds);

    for (int i = 0; i < N; ++i)
    {
        points[i].x = rand_r(&seed) % l;
        points[i].y = rand_r(&seed) % l;
        points[i].r = rand_r(&seed) % (a * b);
    }

    mask_size = l * l * size;
    mask = (int*)malloc(mask_size * sizeof(int));

    if (mask == NULL)
    {
        printf("virtual memory exhausted\n");
        MPI_Finalize();
        return 1;
    }

    for (int i = 0; i < mask_size; ++i) {
        mask[i] = 0;
    }

    MPI_File bin_file;
    MPI_File_delete("data.bin", MPI_INFO_NULL);
    MPI_File_open(MPI_COMM_WORLD, "data.bin", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &bin_file);

    for (int i = 0; i < N; ++i)
    {
        size_t currentX = points[i].x;
        size_t currentY = points[i].y;
        size_t currentR = points[i].r;
        mask[currentY * l * size + currentX * size + currentR] += 1;
    }

    MPI_Aint intex;
    MPI_Aint e;
    MPI_Type_get_extent(MPI_INT, &e, &intex);

    MPI_Datatype view;
    MPI_Type_vector(l, l * size, l * a * size, MPI_INT, &view);
    MPI_Type_commit(&view);

    offset = (curr_proc_x * l + curr_proc_y * a * l * l) * size;
    MPI_File_set_view(bin_file, offset * sizeof(int), MPI_INT, view, "native", MPI_INFO_NULL);

    MPI_File_write(bin_file, mask, mask_size, MPI_INT, MPI_STATUS_IGNORE);
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

        fprintf(stats, "%lu %lu %lu %lu %fs\n", l, a, b, N, full_time);

        fclose(stats);
    }

    free(points);

    MPI_Finalize();

    return 0;
}