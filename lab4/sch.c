#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

/* The data structure to be sent to the worker threads */
typedef struct unit
{
    pthread_t *t;           // list of worker threads
    int n;                  // the number of worker threads
    int *pLoc;              // location where producer will next fill
    int *buffer;            // buffer containing the shared data
    int M;                  // Max buffer size
    int *cLoc;              // location from where consumer will next read.
    int *done;              // keep record of whether the producers are done.
    int count;              // number of data items currently in the buffer
} unit;

/* The data structure to be sent to the scheduler thread */
typedef struct schedulerUnit
{
    unit *u;                // the data structure sent to the worker threads
    float slice;            // time slice to be allotted to each worker thread
    int *cat;               // list that contains category of worker (1 means P, 0 means C)
} sUnit;

/* put the thread to sleep */
void putMeToSleep(int c)
{
        pause();
}

/* wake up the thread */
void wakeMeUp(int c) {}

/* provides pthread id to idx mapping */
int getMyIdx(pthread_t p, pthread_t *list, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (p == list[i])
            return i;
    }
    printf("[ERROR] Thread %ld is not in the threads' list.\n", p);
    exit(1);
}

/* consumer function */
void *consumer(unit *u)
{
    pthread_detach(pthread_self());
    
    signal(SIGUSR1, putMeToSleep);
    signal(SIGUSR2, wakeMeUp);

    int consumed;
    pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
    pause();
    
    while (true)
    {
        pthread_mutex_lock(&mlock);
        
        while ((u->count) == 0)
            ; // buffer is empty, wait
        consumed = u->buffer[*(u->cLoc)];
        u->count--;
        *(u->cLoc) = (*(u->cLoc) + 1) % u->M;
        
        pthread_mutex_unlock(&mlock);
    }
}

/* producer function */
void *producer(unit *u)
{
    pthread_detach(pthread_self());
    
    int idx = getMyIdx(pthread_self(), u->t, u->n);
    signal(SIGUSR1, putMeToSleep);
    signal(SIGUSR2, wakeMeUp);
    
    pthread_mutex_t mlock = PTHREAD_MUTEX_INITIALIZER;
    
    srand(time(NULL));
    pause();
    // create a 1000 integers and add them as and when it gets the chance
    for (int i = 0; i < 1000; i++)
    {
        int generated = rand();
        
        pthread_mutex_lock(&mlock);
        
        while ((u->count) == u->M)
            ;
        u->buffer[*(u->pLoc)] = generated;
        u->count++;
        *(u->pLoc) = (*(u->pLoc) + 1) % u->M;
        
        pthread_mutex_unlock(&mlock);
    }
    u->done[idx] = 1;
    printf("Thread [%ld] is exiting.\n", pthread_self());
    pthread_exit(NULL);
}

/*Scheduler function*/
void *scheduler(sUnit *data)
{
    // pthread_detach(pthread_self());

    printf("\nBeginning scheduler operation...\n\n");
    printf("Number of worker threads: %d\n", data->u->n);
    printf("Time slice allotted to every worker: %.2f second(s)\n\n", data->slice);

    // If in an iteration, none of the producers were woken up
    // flag would be false.
    int flag = true;

    // check whether consumers exist
    bool present = false;
    for (int i = 0; i < data->u->n; i++)
    {
        if (data->cat[i] == 0)
        {
            present = true;
            break;
        }
    }

    // run till no producers have anything left to do and 
    // the buffer is empty (if consumers are present)
    while (flag || (present && data->u->count > 0))
    {
        flag = false;
        for (int i = 0; i < (data->u->n); i++)
        {
            printf("[%ld]\t", data->u->t[i]);
            
            if (data->u->done[i] == 1)
                printf("Producer has already completed its job.\n\n");
            else
            {
                if (data->cat[i] == 1)
                {
                    printf("Waking up producer.\n");
                    flag = true;
                }
                else
                    printf("Waking up consumer.\n");
                
                pthread_kill(data->u->t[i], SIGUSR2);
                usleep(data->slice*1000000);
                pthread_kill(data->u->t[i], SIGUSR1);
                
                printf("\t\t\tBuffer count post execution = %d\n\n", data->u->count);
            }
        }
    }
    
    printf("None of the producers have anything to write.\n");
    printf("Ending all consumers and exiting.\n\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    int n = 0;
    printf("Number of threads to create: ");
    scanf("%d", &n);

    int M = 0;
    printf("Buffer Size: ");
    scanf("%d", &M);

    float q = 1;
    printf("Enter time slice to allot to each thread: ");
    scanf("%f", &q);
    // time slice value

    srand(time(NULL));
    pthread_t tids[n]; // thread IDs
    int category[n];   // producer / consumer

    // create the structure to be shared with the threads
    unit u;
    
    // allocate buffer memory
    u.buffer = (int *)malloc(M * sizeof(int));
    if (u.buffer == NULL)
    {
        printf("Buffer malloc failed.\n");
        exit(1);
    }

    // allocate memory for consumer pointer's current location
    u.cLoc = (int *)malloc(sizeof(int));
    if (u.cLoc == NULL)
    {
        printf("Consumer pointer location malloc failed.\n");
        exit(1);
    }
    *u.cLoc = 0;

    // allocate memory for producer pointer's current location
    u.pLoc = (int *)malloc(sizeof(int));
    if (u.pLoc == NULL)
    {
        printf("Producer pointer location malloc failed.\n");
        exit(1);
    }
    *u.pLoc = 0;
    
    u.count = 0;
    
    u.done = (int *)malloc(n * sizeof(int));
    if (u.done == NULL)
    {
        printf("Producer completeness array malloc failed.\n");
        exit(1);
    }
    for (int i = 0; i < n; i++)
        u.done[i] = 0;
    
    u.n = n;
    u.t = (pthread_t *)malloc(sizeof(pthread_t));
    if (u.t == NULL)
    {
        printf("Pthreads array malloc failed.\n");
        exit(1);
    }
    u.t = tids;
    u.M = M;

    sUnit su;
    su.slice = q;
    su.u = (unit *)malloc(sizeof(unit));
    su.u = &u;
    su.cat = (int *)malloc(sizeof(int));
    su.cat = category;

    // create n new threads
    for (int i = 0; i < n; i++)
    {
        // create a consumer or producer
        int choice = rand() % 2;

        if (choice == 0)
        {
            // creating a consumer
            int r_check = pthread_create(&tids[i], NULL, (void *)consumer, &u);
            if (r_check)
            {
                fprintf(stderr, "\nERROR %d: Failed while tring to create a consumer.\n", r_check);
                exit(1);
            }
            printf("Created a consumer: %ld.\n", tids[i]);
            category[i] = 0;
        }
        else
        {
            // creating a producer
            int r_check = pthread_create(&tids[i], NULL, (void *)producer, &u);
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
    sleep(2); // to make sure all the worker threads have their signal handlers etc. declared.
    int s_check = pthread_create(&sch, NULL, (void *)scheduler, &su);
    if (s_check)
    {
        fprintf(stderr, "\nERROR %d: Failed while tring to create the scheduler thread.\n", s_check);
        exit(1);
    }
    pthread_exit(NULL); /* terminate the thread */
}
