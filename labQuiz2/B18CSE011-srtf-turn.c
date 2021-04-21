#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX(a, b) (a>b)?a:b


void preSJF(int arrivals[], int cpuBursts[], int n) {

    printf("Preemptive shortest job first scheduling:\n\n");

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

    int currTime = 0; int endIdx = 0, s_idx = 0;
    gotForFirstTime[0] = 1;
    whatFirstTime[0] = 0;
    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
    while(1) {
        currTime += 1;
        cpuBursts[s_idx] -= 1;
        if (cpuBursts[s_idx] == 0)
            done[s_idx] = 1;
        
        // add the waiting times
        for (int i=0;i<=endIdx;i++) {
            if (done[i] == 0 && i!=s_idx) {
                waiting[i] += 1;
            }
        }

        if (endIdx<n-1 && arrivals[endIdx+1] == currTime) {
            // new processes have arrived so need to do some checking
            
            endIdx += 1;
            int startIdx = endIdx;
            // if arrival times are same
            while (endIdx<n-1 && arrivals[endIdx+1] == currTime)
                endIdx += 1;
        
            if (done[s_idx]==1)
            {
                printf("[t=%d] Process %d has completed.\n", currTime, s_idx);
                // get the shortest
                int shortest = __INT_MAX__;
                for(int i=0;i<=endIdx;i++)
                {
                    if (done[i] == 0 && shortest > cpuBursts[i])
                    {
                        shortest = cpuBursts[i];
                        s_idx = i;
                    }
                }
                printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
            }
            else {
                // current is not over and the is the best yet
                // so just compare with all the newly arrived

                // get the smallest from the newly arrived.
                int shortest = __INT_MAX__, temp_idx;
                for(int i=startIdx;i<=endIdx;i++)
                {
                    if (done[i] == 0 && shortest > cpuBursts[i])
                    {
                        shortest = cpuBursts[i];
                        temp_idx = i;
                    }
                }
                if (cpuBursts[temp_idx] < cpuBursts[s_idx]){
                    printf("[t=%d] Process %d was preempted.\n", currTime, s_idx);
                    s_idx = temp_idx;
                    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
                }
                else {
                    // s_idx stays the same i.e. continuing with the current process
                }
            }
        }
        else {

            // a new process hasn't arrived.
            // just check if the current is done

            if (done[s_idx]==1){
                printf("[t=%d] Process %d has completed.\n", currTime, s_idx);
                // get the next shortest and if none is there,
                // terminate.
                int shortest = __INT_MAX__, found = 0;
                for(int i=0;i<=endIdx;i++)
                {
                    if (done[i] == 0 && shortest > cpuBursts[i])
                    {
                        shortest = cpuBursts[i];
                        s_idx = i;
                        found = 1;
                    }
                }
                if (found==0 && endIdx == n-1) {
                    printf("[t=%d] No processes left.\n", currTime);
                    break;
                }
                else if (found == 0) {
                    // weren't found, but some are there. so jump to their time
                    endIdx += 1;
                    currTime += arrivals[endIdx] - currTime;

                    // to incorporate for same arrival times
                    while (endIdx<n-1 && arrivals[endIdx+1] == currTime)
                        endIdx += 1;

                    int shortest = __INT_MAX__, found = 0;
                    for (int i = 0; i <= endIdx; i++)
                    {
                        if (done[i] == 0 && shortest > cpuBursts[i])
                        {
                            shortest = cpuBursts[i];
                            s_idx = i;
                        }
                    }
                    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
                }
                else
                    // shortest was found already
                    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
            }
            else {
                // current is not done so continue.
            }
        }

        if (gotForFirstTime[s_idx] == 0)
        {   
            // if the process has got the CPU for the first time
            gotForFirstTime[s_idx] = 1;
            whatFirstTime[s_idx] = currTime - arrivals[s_idx];
            printf("Process %d got the CPU for the first time.\n", s_idx);
        }
    }
    printf("\n");
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
        // printf("%d, ", cpuBursts[i]);
        arrivals[i] = i;
        i++;
    }

    preSJF(arrivals, cpuBursts, n);

}