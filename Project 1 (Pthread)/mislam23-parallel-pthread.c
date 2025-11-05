#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX 1111490560  // Max buffer size (~1GB)

int n1, n2;
char *s1, *s2;
FILE *fp;

int NUM_THREADS;  // Will be set by user input
int *countArray;  // Dynamically allocated based on NUM_THREADS

int readf(FILE *fp) {
    if ((fp = fopen("strings (medium).txt", "r")) == NULL) {
        printf("ERROR: can't open strings (medium).txt!\n");
        return 0;
    }

    s1 = (char *)malloc(sizeof(char) * MAX);
    if (s1 == NULL) {
        printf("ERROR: Out of memory!\n");
        return -1;
    }
    s2 = (char *)malloc(sizeof(char) * MAX);
    if (s2 == NULL) {
        printf("ERROR: Out of memory\n");
        return -1;
    }

    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);
    n2 = strlen(s2) - 1;

    if (s1 == NULL || s2 == NULL || n1 < n2)
        return -1;

    fclose(fp);
    return 0;
}

int num_substring(int tid) {
    int i, j, match_count = 0;
    int chunk_size = n1 / NUM_THREADS;
    int start = tid * chunk_size;
    int end = (tid == NUM_THREADS - 1) ? (n1 - n2) : ((tid + 1) * chunk_size);

    if (end > n1 - n2 + 1) end = n1 - n2 + 1;

    for (i = start; i < end; i++) {
        for (j = 0; j < n2; j++) {
            if (s1[i + j] != s2[j]) break;
        }
        if (j == n2) {
            match_count++;
        }
    }

    countArray[tid] = match_count;
    return match_count;
}


void *calSubStringThread(void *threadid) {
    long tid = (long)threadid;
    int num = num_substring(tid);
    printf("Thread %ld found %d matches.\n", tid, num);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    printf("Enter the number of threads to use: ");
    scanf("%d", &NUM_THREADS);

    if (NUM_THREADS <= 0) {
        printf("ERROR: Number of threads must be positive.\n");
        return -1;
    }

    pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
    countArray = calloc(NUM_THREADS, sizeof(int)); // Initialize to 0

    if (threads == NULL || countArray == NULL) {
        printf("ERROR: Memory allocation for threads failed.\n");
        return -1;
    }

    int t, rc;
    int totalNum = 0;

    if (readf(fp) != 0) {
        exit(-1);
    }

    clock_t start_time = clock();

    for (t = 0; t < NUM_THREADS; t++) {
        rc = pthread_create(&threads[t], NULL, calSubStringThread, (void *)(size_t)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    for (t = 0; t < NUM_THREADS; t++) {
        totalNum += countArray[t];
    }

    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("\nThe number of substrings is: %d\n", totalNum);
    printf("Execution time: %.6f seconds\n", time_taken);

    free(threads);
    free(countArray);
    free(s1);
    free(s2);

    return 0;
}
