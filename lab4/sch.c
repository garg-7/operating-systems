#include <stdio.h> /* standard I/O routines                 */
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#define M 10000 /*Buffer size*/

/* The data structure to hold the buffer
and related pointers. */
typedef struct unit {
    int *pLoc;
    int *buffer;
    int *cLoc;
    int count;
} unit;

typedef struct schedulerUnit {
    pthread_t *t;
    int n;
    int slice;
    unit *u;
    int *cat;
} sUnit;

// put the thread to sleep 
void putMeToSleep(int c){
    pause();
}

// wake up the thread
void wakeMeUp(int c){}

// consumer function
void* consumer(unit *u)
{   
    // printf("Got here11.\n");
    pthread_detach(pthread_self());
    signal(SIGUSR1, putMeToSleep);
    signal(SIGUSR2, wakeMeUp);
    pause();
    int consumed;
    pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
    // printf("Got here7.\n");
    while(true) {
        // printf("hi1.\n");
        pthread_mutex_lock(&mlock);
        while ((u->count) == 0); // buffer is empty, wait
        consumed = u->buffer[*(u->cLoc)];
        u->count --;
        *(u->cLoc)=(*(u->cLoc)+1)%M;
        pthread_mutex_unlock(&mlock);
        // printf("hi2.\n");
    }
    // printf("Got here8.\n");
}

// producer thread
void* producer(unit *u)
{   
    pthread_detach(pthread_self());
    signal(SIGUSR1, putMeToSleep);
    signal(SIGUSR2, wakeMeUp);
    pause();
    pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
    srand(time(NULL));
    // create a 1000 integers and add them as and when you 
    // get the chance
    // printf("Got here4.\n");
    for(int i=0;i<1000;i++){
        int generated = rand();
        pthread_mutex_lock(&mlock);
        while((u->count) ==M);
        u->buffer[*(u->pLoc)] = generated;
        u->count ++;
        *(u->pLoc)=(*(u->pLoc)+1)%M;
        pthread_mutex_unlock(&mlock);
    }
    while(1);
    // printf("Got here6.\n");
}

// scheduling thread
void* scheduler(sUnit * data) {
    pthread_detach(pthread_self());
    // printf("Got here5.\n");
    printf("n: %d\n", data->n);
    printf("count: %d\n", data->u->count);
    printf("data[0]: %ld\n", data->t[0]);
    int done = false;
    while(1){
        for(int i=0;i<(data->n);i++){
            if (data->cat[i]==1)
            printf("Waking up producer [%ld]\n", data->t[i]);
            else 
            printf("Waking up consumer [%ld]\n", data->t[i]);
            int prev_count = data->u->count;
            pthread_kill(data->t[i], SIGUSR2);
            // printf("Got here00.\n");
            sleep(data->slice);
            pthread_kill(data->t[i], SIGUSR1);
            int new_count = data->u->count;
            printf("[Paused thread: %ld], ", data->t[i]);
            printf("Buffer count = %d\n",  data->u->count);
            if(data->cat[i]==1 && new_count==prev_count && new_count == 0){
                done = true;
                break;
            }
        }
        if (done==true)
        break;
    }
    exit(0);
    // printf("Got here3.\n");
}

/* like any C program, program's execution begins in main */
int main(int argc, char *argv[])
{
    int n = 0;
    printf("Number of threads to create: ");
    scanf("%d", &n);

    // time slice value
    int q = 1;

    srand(time(NULL));
    pthread_t tids[n]; // thread IDs
    int category[n];    // producer / consumer
    
    // create the shared buffer
    unit u;
    u.buffer = (int*)malloc(M*sizeof(int));
    u.cLoc = (int*)malloc(sizeof(int));
    *u.cLoc = 0;
    u.pLoc = (int*)malloc(sizeof(int));
    *u.pLoc = 0;
    u.count = 0;

    sUnit su;
    su.n = n;
    su.slice = q;
    su.t = (pthread_t*)malloc(sizeof(pthread_t));
    su.t = tids;
    su.u = (unit*)malloc(sizeof(unit));
    su.u = &u;
    su.cat = (int*)malloc(sizeof(int));
    su.cat = category;
    // create n new threads
    for (int i = 0; i < n; i++)
    {
        // create a consumer or producer
        int choice = rand() % 2;
        // printf("%d ", choice);
        
        if (choice == 0)
        {
            // creating a consumer
            int r_check = pthread_create(&tids[i], NULL, (void*)consumer, &u);
            if (r_check)
            {
                fprintf(stderr, "\nERROR %d: Failed while tring to create a consumer.\n", r_check);
                exit(1);
            }
            printf("Created a consumer: %ld.\n", tids[i]);
            category[i]=0;
        }
        else
        {
            // creating a producer
            int r_check = pthread_create(&tids[i], NULL, (void*)producer, &u);
            if (r_check)
            {
                fprintf(stderr, "\nERROR %d: Failed while tring to create a producer.\n", r_check);
                exit(1);
            }
            printf("Created a producer: %ld.\n", tids[i]);
            category[i] = 1;
        }
    }

    // the Scheduler thread
    pthread_t sch;
    sleep(3);
    int s_check = pthread_create(&sch, NULL, (void*)scheduler, &su);
    // printf("\nCreated new thread (%lu) ... \n", thread_id);
    pthread_join(sch, NULL);
    pthread_exit(NULL); /* terminate the thread */
    printf("Got here2.\n");
}
