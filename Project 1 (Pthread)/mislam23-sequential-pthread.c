#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>  // For measuring execution time

// #define MAX 10240 // 
#define MAX 1111490560 // 

int total = 0;
int n1, n2;
char *s1, *s2;
FILE *fp;

int readf(FILE *fp)
{
    if ((fp = fopen("strings.txt", "r")) == NULL) {
        printf("ERROR: can't open string.txt!\n");
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
    // Read s1 and s2 from the file
    s1 = fgets(s1, MAX, fp);
    s2 = fgets(s2, MAX, fp);
    n1 = strlen(s1);         // Length of s1
    n2 = strlen(s2) - 1;     // Remove newline from s2

    if (s1 == NULL || s2 == NULL || n1 < n2)
        return -1;

    fclose(fp);
    return 0;
}

int num_substring(void)
{
    int i, j, k;
    int count;

    total = 0;  // Ensure total is reset
    for (i = 0; i <= (n1 - n2); i++) {
        count = 0;
        for (j = i, k = 0; k < n2; j++, k++) {
            if (*(s1 + j) != *(s2 + k)) {
                break;
            } else {
                count++;
            }
            if (count == n2) {
                total++;
            }
        }
    }
    return total;
}

int main(int argc, char *argv[])
{
    int count;

    // Start measuring time
    clock_t start_time = clock();

    readf(fp);
    count = num_substring();

    // End measuring time
    clock_t end_time = clock();
    double time_taken = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    printf("The number of substrings is: %d\n", count);
    printf("Execution time: %.6f seconds\n", time_taken);

    return 0;
}
