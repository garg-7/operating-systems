#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX(a, b) (a>b)?a:b

void FCFS(int arrivals[], int cpuBursts[], int n)
{
    int waiting[n];
    int turnaround[n];
    for (int i=0;i<n;i++){
        turnaround[i] = 0;
        waiting[i] = 0;
    }
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
    // free(waiting);
    // free(turnaround);
    return;
}

void nonPreSJF(int arrivals[], int cpuBursts[], int n)
{
    int done[n], waiting[n];
    for (int i=0;i<n;i++){
        done[i] = 0;
        waiting[i] = 0;
    }
    int endIdx = 0;
    int currTime = 0;

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

    int turnaround[n];
    for (int i=0;i<n;i++)
    turnaround[i] = 0;
    float avgWaiting = 0, avgTurnaround = 0;
    

    for (int i = 0; i < n; i++)
    {
        avgWaiting += waiting[i];
        turnaround[i] = waiting[i] + cpuBursts[i];
        avgTurnaround += turnaround[i];
    }

    avgWaiting /= n;
    avgTurnaround /= n;
    // for(int i=0;i<n;i++){
    //     printf("%d, %d, %d\n", arrivals[i], cpuBursts[i], waiting[i]);
    // }

    printf("Average waiting time: %.2f seconds\n", avgWaiting);
    printf("Average turnaround time: %.2f seconds\n", avgTurnaround);
    printf("Average response time: %.2f seconds\n", avgWaiting);
    // free(waiting);
    // free(done);
    // free(turnaround);
    return;
}

void preSJF(int arrivals[], int cpuBursts[], int n) {
    int oldCpuBursts[n];
    int done[n];
    int waiting[n];
    int gotForFirstTime[n];
    int whatFirstTime[n];
    
    for (int i=0;i<n;i++){
        done[i] = 0;
        waiting[i] = 0;
        gotForFirstTime[i] = 0;
        whatFirstTime[i] = 0;
        oldCpuBursts[i] = cpuBursts[i];
    }

    int currTime = 0; int endIdx = 0;
    while(1){
        // select the job with least remaining time and run it
        // till another job arrives. compare with this new job.
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
        if (gotForFirstTime[s_idx] == 0)
        {   
            // if the process has got the CPU for the first time
            gotForFirstTime[s_idx] = 1;
            whatFirstTime[s_idx] = currTime - arrivals[s_idx];
        }
        if (endIdx<n-1){
            int timeToRun = arrivals[endIdx+1] - currTime;
            cpuBursts[s_idx] -= timeToRun;
            cpuBursts[s_idx] = MAX(0, cpuBursts[s_idx]);
            currTime += timeToRun;
            // arrivals[s_idx] = currTime;
            if (cpuBursts[s_idx] == 0) {
                done[s_idx] = 1;
            }
            for(int i=0;i<=endIdx;i++)
            {
                if (done[i] == 0 && i!=s_idx )
                    waiting[i] += timeToRun;
            }
        }
        else {
            // all processes are already in so whatever was selected, has to be run in its entirety
            int timeToRun = cpuBursts[s_idx];
            cpuBursts[s_idx] = 0;
            done[s_idx] = 1;
            currTime += timeToRun;
            for(int i=0;i<=endIdx;i++)
            {
                if (done[i] == 0 && i!=s_idx )
                    waiting[i] += timeToRun;
            }
        }

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
    
    int turnaround[n];
    for (int i=0;i<n;i++)
    turnaround[i] = 0;
    float avgWaiting = 0, avgTurnaround = 0, avgResponse = 0;
    

    for (int i = 0; i < n; i++)
    {
        avgWaiting += waiting[i];
        turnaround[i] = waiting[i] + oldCpuBursts[i];
        avgTurnaround += turnaround[i];
        avgResponse += whatFirstTime[i];
    }
    avgWaiting /= n;
    avgTurnaround /= n;
    avgResponse /= n;
    // for(int i=0;i<n;i++){
    //     printf("%d, %d, %d\n", arrivals[i], cpuBursts[i], waiting[i]);
    // }

    printf("Average waiting time: %.2f seconds\n", avgWaiting);
    printf("Average turnaround time: %.2f seconds\n", avgTurnaround);
    printf("Average response time: %.2f seconds\n", avgResponse);
    // free(waiting);
    // free(done);
    // free(turnaround);
}

void RR(int arrivals[], int cpuBursts[], int n, int q) {}

void priority(int arrivals[], int cpuBursts[], int priorities[], int n) {}

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

    int arrivals[n];
    int priorities[n];
    int cpuBursts[n];

    fseek(f, 0, SEEK_SET);

    int i = 0;
    while (fscanf(f, "%d,%d,%d", &arrivals[i], &priorities[i], &cpuBursts[i]) > 0)
    {
        i++;
    }

    FCFS(arrivals, cpuBursts, n);
    nonPreSJF(arrivals, cpuBursts, n);
    preSJF(arrivals, cpuBursts, n);
    int quantum = 2;
    RR(arrivals, cpuBursts, n, quantum);
    priority(arrivals, cpuBursts, priorities, n);

    // for(int i=0;i<n;i++){
    //     printf("%d, %d, %d\n", arrivals[i], cpuBursts[i], waiting[i]);
    // }
}