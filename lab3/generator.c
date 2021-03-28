#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main()
{
    srand(time(NULL));
    
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    float mu;
    printf("Enter the mean of exponential distribution: ");
    scanf("%f", &mu);
    if (mu == 0)
    {
        fprintf(stderr, "Invalid mu.\n");
        exit(1);
    }

    int *arrivals = (int *)malloc(n * sizeof(int));
    int *priorities = (int *)malloc(n * sizeof(int));
    int *cpuBursts = (int *)malloc(n * sizeof(int));

    arrivals[0] = 0;
    for (int i = 0; i < n; i++)
    {
        if (i > 0)
            arrivals[i] = arrivals[i - 1] + (int)(-1 * log((float)rand() / (float)RAND_MAX) / mu * 9 + 1);
        priorities[i] = rand() % (10 + 1 - 1) + 1;   // between 1 and 10
        cpuBursts[i] = rand() % (20 + 1 - 1) + 1;   // between 1 and 20
    }

    FILE *f = fopen("process_arrival_trace.txt", "w");
    for (int i = 0; i < n; i++)
    {
        fprintf(f, "%d,%d,%d\n", arrivals[i], priorities[i], cpuBursts[i]);
    }
}