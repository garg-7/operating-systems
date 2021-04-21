#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>



void* test_thread(int* a){
    printf("I am thread %d\n", *a);
    sleep(1);
}

int main()
{
    int n = 5;
    pthread_t pids[n];
    int positions[n];
    for(int i=0;i<n;i++){
        positions[i] = i;
        int tCheck = pthread_create(&pids[i], NULL, (void*)test_thread, &positions[i]);
        if (tCheck){
            fprintf(stderr, "\nERROR %d: Failed while tring to create a thread.\n", tCheck);
            exit(1);
        }
    }
    pthread_join(pids[n-1], NULL);
    printf("All threads finished\n");
    exit(0);
}