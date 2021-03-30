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

void nonPreSJF(int arrivals[], int cpuBursts[], int n)
{
    printf("Non-preemptive shortest job first scheduling:\n\n");
    int done[n], waiting[n], oldCpuBursts[n];
    for (int i=0;i<n;i++){
        done[i] = 0;
        waiting[i] = 0;
        oldCpuBursts[i] = cpuBursts[i];
    }
    int endIdx = 0;
    int currTime = 0;
    int s_idx = 0;

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
            // process arrived in that one second
            endIdx += 1;
        
        // if the current process is done, do something new
        if (cpuBursts[s_idx] == 0) {
            done[s_idx] = 1;
            // pick the next process
            int shortest = __INT_MAX__, found = 0;
            for (int i = 0; i <= endIdx; i++)
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
            else if (found==0)
            {
                // some are left but not found. So jump in time to the next
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
                printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
            }
            else {
                printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
            }
        }
    }
    printf("\n");
    int turnaround[n];
    for (int i=0;i<n;i++)
    turnaround[i] = 0;
    float avgWaiting = 0, avgTurnaround = 0;
    

    for (int i = 0; i < n; i++)
    {
        avgWaiting += waiting[i];
        turnaround[i] = waiting[i] + oldCpuBursts[i];
        avgTurnaround += turnaround[i];
    }

    avgWaiting /= n;
    avgTurnaround /= n;

    printf("Average waiting time: %.2f seconds\n", avgWaiting);
    printf("Average turnaround time: %.2f seconds\n", avgTurnaround);
    printf("Average response time: %.2f seconds\n", avgWaiting);
    return;
}


void pre2(int arrivals[], int cpuBursts[], int n) {

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
    printf("[t=%d] Process 0 is picked.\n", currTime);
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
                printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
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
                    s_idx = temp_idx;
                    printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
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
                    printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
                }
                else
                    // shortest was found already
                    printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
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

void RR(int arrivals[], int cpuBursts[], int n, int slice) {
    
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

    int currTime = 0;
    gotForFirstTime[0] = 1;
    whatFirstTime[0] = 0;   

    int s_idx = 0, endIdx = 0;
    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
    int currSlice = 0;
    while(1) {
        currTime += 1;
        currSlice += 1;
        cpuBursts[s_idx] -= 1;
        
        for (int i=0;i<=endIdx;i++) {
            if (done[i] == 0 && i!=s_idx) {
                waiting[i] += 1;
            }
        }

        while (endIdx<n-1 && arrivals[endIdx+1] == currTime)
            // processes arrived in the meantime
            endIdx += 1;
        
        // if the time slice is up
        if (currSlice == slice) {
            currSlice = 0;
            
            // get the next process
            if (cpuBursts[s_idx]==0){
                done[s_idx] = 1;
            }
            else {
                // so that current is at the back of the queue
                arrivals[s_idx] = arrivals[endIdx] + 1;
            }
            int youngest = __INT_MAX__, found = 0;
            for(int i=0;i<=endIdx;i++){
                if(done[i]==0 && youngest > arrivals[i]){
                    youngest = arrivals[i];
                    s_idx = i;
                    found = 1;
                }
            }
            if (found==0 && endIdx == n-1){
                // no process left
                printf("[t=%d] No processes left.\n", currTime);
                break;
            }
            else if (found==0){
                // a time gap and everything till this time is done
                endIdx += 1;
                s_idx += 1;
                currTime += arrivals[s_idx] - currTime;
                printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
            }
            else {
                // shortest was found already
                printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
            }
        }
        else {
            // time slice is not done.
            // check if current process is done

            if (cpuBursts[s_idx]==0){
                // current is done
                currSlice = 0;
                done[s_idx] = 1;
                int youngest = __INT_MAX__, found = 0;
                for(int i=0;i<=endIdx;i++){
                    if(done[i]==0 && youngest > arrivals[i]){
                        youngest = arrivals[i];
                        s_idx = i;
                        found = 1;
                    }
                }
                if (found==0 && endIdx == n-1){
                    // no process left
                    printf("[t=%d] No processes left.\n", currTime);
                    break;
                }
                else if (found==0){
                    // a time gap and everything till this time is done
                    endIdx += 1;
                    s_idx += 1;
                    currTime += arrivals[s_idx] - currTime;
                    printf("[t=%d] Process %d was chosen.\n", currTime, s_idx);
                }
                else {
                    // shortest was found already
                    printf("[t=%d] Process %d is picked.\n", currTime, s_idx);
                }
            }
            else {
                // time slice not done and current also not done. 
                // continue with the current.
            }

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

void priority(int arrivals[], int cpuBursts[], int priorities[], int n) {
    
    printf("Non-preemptive priority based scheduling:\n\n");

    int done[n], waiting[n];
    for (int i=0;i<n;i++){
        done[i] = 0;
        waiting[i] = 0;
    }
    int endIdx = 0;
    int currTime = 0;

    while (1)
    {
        // get the highest priority unfinished job
        // highest priority is the one with the lowest number
        int highest = __INT_MAX__, s_idx = -1;
        for (int i = 0; i <= endIdx; i++)
        {
            if (done[i] == 0 && highest > priorities[i])
            {
                highest = priorities[i];
                s_idx = i;
            }
        }
        printf("Process %d with priority %d was chosen.\n", s_idx, priorities[s_idx]);
        waiting[s_idx] = currTime - arrivals[s_idx];
        printf("It was allotted CPU for %d s and it's done.\n", cpuBursts[s_idx]);
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
        printf("\n");
    }
    printf("\n");
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

    // FCFS(arrivals, cpuBursts, n);
    // nonPreSJF(arrivals, cpuBursts, n);
    // preSJF(arrivals, cpuBursts, n);
    // pre2(arrivals, cpuBursts, n);
    int quantum = 2;
    RR(arrivals, cpuBursts, n, quantum);
    // priority(arrivals, cpuBursts, priorities, n);

    // for(int i=0;i<n;i++){
    //     printf("%d, %d, %d\n", arrivals[i], cpuBursts[i], waiting[i]);
    // }
}