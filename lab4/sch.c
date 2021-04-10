#include <stdio.h> /* standard I/O routines                 */
#include <stdlib.h>
#include <pthread.h> /* pthread functions and data structures */
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#define M 10000 /*Buffer size*/

/* The data structure to be sent to the worker threads */
typedef struct unit {
    pthread_t *t;   // list of worker threads
    int n;          // the number of worker threads
    int *pLoc;      // location where producer will next fill
    int *buffer;    // buffer containing the shared data
    int *cLoc;      // location from where consumer will next read.
    int *done;      // keep record of whether the producers are done.
    int count;      // number of data items currently in the buffer
} unit;

/* The data structure to be sent to the scheduler thread */
typedef struct schedulerUnit {
    unit *u;        // the data structure sent to the worker threads
    int slice;      // time slice to be allotted to each worker thread
    int *cat;       // list that contains category of worker (1 means P, 0 means C)
} sUnit;

/* put the thread to sleep */ 
void putMeToSleep(int c){
    pause();
}

/* wake up the thread */
void wakeMeUp(int c){}


int getMyIdx(pthread_t p, pthread_t *list, int n){
    for(int i=0;i<n;i++)
    {
        if (p == list[i])
            return i;
    }
    printf("[ERROR] Thread %ld is not in the threads' list.\n", p);
    exit(1);
}

/* consumer function */
void* consumer(unit *u)
{   
    // printf("Got here11.\n");
    pthread_detach(pthread_self());
    signal(SIGUSR1, putMeToSleep);
    signal(SIGUSR2, wakeMeUp);
    int consumed;
    pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
    pause();
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

/* producer function */
void* producer(unit *u)
{   
    pthread_detach(pthread_self());
    int idx = getMyIdx(pthread_self(), u->t, u->n);
    signal(SIGUSR1, putMeToSleep);
    signal(SIGUSR2, wakeMeUp);
    pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
    srand(time(NULL));
    pause();
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
    u->done[idx] = 1;
    // printf("Got here6.\n");
}

/*Scheduler function*/
void* scheduler(sUnit * data) {
    printf("\nBeginning scheduler operation...\n\n");
    pthread_detach(pthread_self());
    // printf("Got here5.\n");
    printf("Number of worker threads: %d\n", data->u->n);
    printf("Time slice allotted to every worker: %d second(s)\n\n", data->slice);
    // printf("data[0]: %ld\n", data->t[0]);
    
    // If in an iteration, none of the producers were woken up
    // flag would be false.
    int flag = true;

    while(flag){
        flag = false;
        for(int i=0;i<(data->u->n);i++){
            int prev_count = data->u->count;
            if (data->u->done[i]==1)
                printf("Producer [%ld] has already completed its job.\n\n", data->u->t[i]);
            else {
                if (data->cat[i]==1) {
                    printf("Waking up producer [%ld]\n", data->u->t[i]);
                    flag = true;
                }
                else 
                    printf("Waking up consumer [%ld]\n", data->u->t[i]);
                pthread_kill(data->u->t[i], SIGUSR2);
                sleep(data->slice);
                pthread_kill(data->u->t[i], SIGUSR1);
                printf("[Paused thread: %ld], ", data->u->t[i]);
                printf("Buffer count = %d\n\n",  data->u->count);
            }
        }
    }
    printf("\nNone of the producers have anything to write.\n");
    printf("Exiting.\n\n");
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
    pthread_t tids[n];  // thread IDs
    int category[n];    // producer / consumer
    
    // create the shared buffer
    unit u;
    u.buffer = (int*)malloc(M*sizeof(int));
    u.cLoc = (int*)malloc(sizeof(int));
    *u.cLoc = 0;
    u.pLoc = (int*)malloc(sizeof(int));
    *u.pLoc = 0;
    u.count = 0;
    u.done = (int*)malloc(n*sizeof(int));
    for(int i=0;i<n;i++)
        u.done[i] = 0;
    u.n = n;
    u.t = (pthread_t*)malloc(sizeof(pthread_t));
    u.t = tids;
    
    sUnit su;
    su.slice = q;
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
