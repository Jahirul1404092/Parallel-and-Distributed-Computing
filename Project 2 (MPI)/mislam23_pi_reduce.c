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
        fp = fopen("output_reduce.txt", "w");
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
        double local_sum = 0.0, total_sum = 0.0, pi;
        double start_time, end_time;

        MPI_Barrier(MPI_COMM_WORLD);
        start_time = MPI_Wtime();

        for (long long int i = local_start; i < local_end; i++) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }

        MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        end_time = MPI_Wtime();

        if (rank == 0) {
            pi = step * total_sum;
            double elapsed = end_time - start_time;
            printf("NUMSTEPS = %lld -> PI = %.15f | Time = %.6f s\n", NUMSTEPS, pi, elapsed);
            fprintf(fp, "%lld\t%.15f\t%.6f\n", NUMSTEPS, pi, elapsed);
        }
    }

    if (rank == 0) {
        fclose(fp);
        printf("All results saved in output_reduce.txt\n");
    }

    MPI_Finalize();
    return 0;
}
