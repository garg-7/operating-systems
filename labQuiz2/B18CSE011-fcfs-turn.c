#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX(a, b) (a>b)?a:b

void FCFS(int arrivals[], int cpuBursts[], int n)
{
    printf("First come first serve scheduling:\n\n");

    int waiting[n], oldCpuBursts[n];
    int turnaround[n], done[n];
    for (int i=0;i<n;i++){
        turnaround[i] = 0;
        waiting[i] = 0;
        done[i]= 0;
        oldCpuBursts[i] = cpuBursts[i];
    }
    int currTime = 0;
    float avgWaiting = 0, avgTurnaround = 0;


    int s_idx = 0, endIdx = 0;
    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
    while(1) {
        currTime += 1;
        cpuBursts[s_idx] -= 1;
        
        for (int i=0;i<=endIdx;i++) {
            if (done[i] == 0 && i!=s_idx) {
                waiting[i] += 1;
            }
        }

        while (endIdx<n-1 && arrivals[endIdx+1] == currTime)
            // processes arrived in the meantime
            endIdx += 1;
        
        // if the current process is done, do something new
        if (cpuBursts[s_idx] == 0) {
            done[s_idx] = 1;
            printf("[t=%d] Process %d has completed.\n", currTime, s_idx);
            // pick the next process
            if (endIdx == s_idx) {
                // no new process has come 
                if (endIdx == n-1) {
                    // no process left
                    printf("[t=%d] No processes left.\n", currTime);
                    break;
                }
                else {
                    endIdx += 1;
                    s_idx += 1;
                    currTime += arrivals[s_idx] - currTime;
                    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
                }
            }
            else {
                // a new process has come
                s_idx += 1;
                printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
            }
        }
    }
    printf("\n");
    for (int i = 0; i < n; i++)
    {
        turnaround[i] = waiting[i] + oldCpuBursts[i];
        
        avgWaiting += waiting[i];
        avgTurnaround += turnaround[i];
    }

    avgWaiting /= n;
    avgTurnaround /= n;
    printf("Average waiting time: %.2f seconds\n", avgWaiting);
    printf("Average turnaround time: %.2f seconds\n", avgTurnaround);
    printf("Average response time: %.2f seconds\n", avgWaiting);

    return;
}

int main(int argc, char *argv[])
{
    FILE *f = fopen("Quiz2-q1.txt", "r");

    int a, b, c, n = 0;
    n = 250;

    int arrivals[n];
    int cpuBursts[n];

    int i = 0;
    // now get the burst time values
    while (fscanf(f, "%d,",&cpuBursts[i]) > 0)
    {
        arrivals[i] = i;
        i++;
    }

    FCFS(arrivals, cpuBursts, n);
}