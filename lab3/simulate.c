#include <stdio.h>
#include <stdlib.h>

void FCFS(int *arrivals, int *cpuBursts, int n)
{
    int *waiting = (int *)malloc(n * sizeof(int));
    int *turnaround = (int *)malloc(n * sizeof(int));
    int currTime = 0;
    float avgWaiting = 0, avgTurnaround = 0;

    for (int i = 0; i < n; i++)
    {
        avgWaiting += currTime - arrivals[i];
        waiting[i] = currTime - arrivals[i];
        avgTurnaround += waiting[i] + cpuBursts[i];
        turnaround[i] = waiting[i] + cpuBursts[i];
        currTime += cpuBursts[i];
    }

    avgWaiting /= n;
    avgTurnaround /= n;
    printf("Average waiting time: %.2f seconds\n", avgWaiting);
    printf("Average turnaround time: %.2f seconds\n", avgTurnaround);
    printf("Average response time: %.2f seconds\n", avgWaiting);
    free(waiting);
    free(turnaround);
    return;
}

void nonPreSJF(int *arrivals, int *cpuBursts, int n)
{
    int *done = (int *)malloc(n * sizeof(int));
    int *waiting = (int *)malloc(n * sizeof(int));
    int endIdx = 0;
    int currTime = 0;
    // int processList[n];
    for (int i = 0; i < n; i++)
    {
        done[i] = 0;
        waiting[i] = 0;
    }
    for(int i=0;i<n;i++){
        printf("%d, %d\n", arrivals[i], cpuBursts[i]);
    }

    while (1)
    {
        // get the shortest unfinished job
        int shortest = __INT_MAX__, s_idx = -1;
        for (int i = 0; i <= endIdx; i++)
        {
            if (done[i] == 0 && shortest > cpuBursts[i])
            {
                shortest = cpuBursts[i];
                s_idx = i;
            }
        }

        waiting[s_idx] = currTime - arrivals[s_idx];
        currTime += cpuBursts[s_idx];
        done[s_idx] = 1;
        
        int allDone = 1;
        
        for (int i = 0; i < n; i++)
        {
            if (done[i] == 0)
            {
                allDone = 0;
                break;
            }
        }
        if (allDone == 1)
            break;
        while (endIdx < n - 1 && arrivals[endIdx + 1] <= currTime)
            endIdx++;
    }

    int *turnaround = (int *)malloc(n * sizeof(int));
    float avgWaiting = 0, avgTurnaround = 0;
    

    for (int i = 0; i < n; i++)
    {
        avgWaiting += waiting[i];
        turnaround[i] = waiting[i] + cpuBursts[i];
        avgTurnaround += turnaround[i];
    }

    avgWaiting /= n;
    avgTurnaround /= n;
    for(int i=0;i<n;i++){
        printf("%d, %d, %d\n", arrivals[i], cpuBursts[i], waiting[i]);
    }

    printf("Average waiting time: %.2f seconds\n", avgWaiting);
    printf("Average turnaround time: %.2f seconds\n", avgTurnaround);
    printf("Average response time: %.2f seconds\n", avgWaiting);
    free(waiting);
    free(done);
    free(turnaround);
    return;
}

void preSJF(int *arrivals, int *cpuBursts, int n) {}

void RR(int *arrivals, int *cpuBursts, int n, int q) {}

void priority(int *arrivals, int *cpuBursts, int *priorities, int n) {}

int main()
{
    FILE *f = fopen("process_arrival_trace.txt", "r");

    int a, b, c, n = 0;
    while (fscanf(f, "%d,%d,%d", &a, &b, &c) > 0)
    {
        n++;
    }
    // printf("%d\n", n);
    // exit(1);

    int *arrivals = (int *)malloc(n * sizeof(int));
    int *priorities = (int *)malloc(n * sizeof(int));
    int *cpuBursts = (int *)malloc(n * sizeof(int));

    fseek(f, 0, SEEK_SET);

    int i = 0;
    while (fscanf(f, "%d,%d,%d", &arrivals[i], &priorities[i], &cpuBursts[i]) > 0)
    {
        i++;
    }

    // FCFS(arrivals, cpuBursts, n);
    nonPreSJF(arrivals, cpuBursts, n);
    preSJF(arrivals, cpuBursts, n);
    int quantum = 2;
    RR(arrivals, cpuBursts, n, quantum);
    priority(arrivals, cpuBursts, priorities, n);

    // for(int i=0;i<n;i++){
    //     printf("%d, %d, %d\n", arrivals[i], cpuBursts[i], waiting[i]);
    // }
}