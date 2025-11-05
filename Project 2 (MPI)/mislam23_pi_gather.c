#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    FILE *fp = NULL;
    if (rank == 0) {
        fp = fopen("output_gather.txt", "w");
        if (!fp) {
            printf("Error opening output file!\n");
            MPI_Finalize();
            return 1;
        }
        fprintf(fp, "NUMSTEPS\tPI\tElapsed_time_sec\n");
    }

    for (long long int NUMSTEPS = 1000; NUMSTEPS <= 1000000000000LL; NUMSTEPS *= 10) {
        double step = 1.0 / (double) NUMSTEPS;
        long long int chunk = NUMSTEPS / size;
        long long int local_start = rank * chunk;
        long long int local_end = (rank == size - 1) ? NUMSTEPS : local_start + chunk;
        double local_sum = 0.0, local_pi = 0.0, pi_total = 0.0;
        double start_time, end_time;
        double *partial_results = NULL;

        MPI_Bcast(&NUMSTEPS, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        start_time = MPI_Wtime();

        for (long long int i = local_start; i < local_end; i++) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }

        local_pi = local_sum * step;

        if (rank == 0)
            partial_results = (double*)malloc(size * sizeof(double));

        MPI_Gather(&local_pi, 1, MPI_DOUBLE, partial_results, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        end_time = MPI_Wtime();

        if (rank == 0) {
            for (int j = 0; j < size; j++)
                pi_total += partial_results[j];

            double elapsed = end_time - start_time;
            printf("NUMSTEPS = %lld -> PI = %.15f | Time = %.6f s\n", NUMSTEPS, pi_total, elapsed);
            fprintf(fp, "%lld\t%.15f\t%.6f\n", NUMSTEPS, pi_total, elapsed);
            free(partial_results);
        }
    }

    if (rank == 0) {
        fclose(fp);
        printf("All results saved in output_gather.txt\n");
    }

    MPI_Finalize();
    return 0;
}
