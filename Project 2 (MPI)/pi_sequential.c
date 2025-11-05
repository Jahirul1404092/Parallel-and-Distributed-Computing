#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE *fp = fopen("output_sequential.txt", "w");
    if (!fp) {
        printf("Error opening output file!\n");
        return 1;
    }

    fprintf(fp, "NUMSTEPS\tPI\tElapsed_time_ns\n");

    for (long long int NUMSTEPS = 1000; NUMSTEPS <= 1000000000LL; NUMSTEPS *= 10) {
        int i;
        double x, pi, sum = 0.0;
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);
        double step = 1.0 / (double) NUMSTEPS;
        x = 0.5 * step;

        for (i = 0; i <= NUMSTEPS; i++) {
            x += step;
            sum += 4.0 / (1.0 + x * x);
        }

        pi = step * sum;
        clock_gettime(CLOCK_MONOTONIC, &end);
        unsigned long long diff = 1000000000L * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);

        printf("NUMSTEPS = %lld -> PI = %.15f | Time = %llu ns\n", NUMSTEPS, pi, diff);
        fprintf(fp, "%lld\t%.15f\t%llu\n", NUMSTEPS, pi, diff);
    }

    fclose(fp);
    printf("All results saved in output_sequential.txt\n");
    return 0;
}
