#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#define CUSTOMERS 7
#define WAKEUP SIGUSR1
#define SLEEPNOW SIGUSR2

pthread_mutex_t tokenLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wAreaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sofaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t barberLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cleaningLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cashierLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct gkStuct{
    pthread_t *customers;   // list of customer IDs
    pthread_t *barbers;     // list of barber IDs
    int bNo;                // number of barbers
    int *tokens;            // number of tokens
    int custNo;             // number of customers
    int *customerServed;    // list containing whether a customer has been served
    int everyoneServed;     // if all the customers have been served
    int custWoken;          // number of customers who have been given entry
    int *woken;             // is ith customer has been given entry in the shop
} gkStruct;

typedef struct barberStruct{
    gkStruct* gk;           // gatekeeper struct
    pthread_t *bChairs;     // list of IDs of the persons in the barber chairs
    int *acceptingPayment;  // list of barbers to keep a record whether one is accepting payment - since cash register is a shared resource
    int* gotPayment;        // list of barbers to keep a record of whether ith barber has taken the payment - so that the customer can leave
    float *amountEarned;    // simply a counter of number of customers served by every barber - extra statistic
} barberStruct;

typedef struct customerStruct{
    gkStruct* gk;           // gatekeeper struct
    barberStruct* b;        // barber struct
    int sofaCurrent;        // number of seats available currently on waiting room sofa
    int waitingCurrent;     // number of places available currently in the waiting room standing area
    int barberCurrent;      // number of barbers currently not busy
    int *stages;            // to note at which stage each customer in the queue is - helps prevent starvation
} customerStruct;


// get index from thread ID
int getIdx(pthread_t * c, int n, pthread_t curr){
    for(int i=0;i<n;i++){
        if (c[i]==curr)
            return i;
    }
    printf("[ERROR] Thread %ld is not in the list.\n", curr);
    exit(3);
}


/* Gatekeeper has given you the token.
Occupy a seat in the waiting area */
void enterShop(customerStruct *c, int selfIdx){
    if (getIdx(c->gk->customers, c->gk->custNo, pthread_self())==0){
        while(true){
            pthread_mutex_lock(&wAreaLock);
            if (c->waitingCurrent==0);
            else {
                c->waitingCurrent -= 1;
                printf("Customer %d collects a token from the gatekeeper.\n", selfIdx);
                printf("Customer %d enters the barbershop.\n", selfIdx);
                printf("Customer %d is standing in the waiting area.\n", selfIdx);
                pthread_mutex_unlock(&wAreaLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 1;
                break;
            }
            pthread_mutex_unlock(&wAreaLock);
        }
    }
    else { 
        // to prevent starvation
        while(c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())-1]<1);
        while(true){
            pthread_mutex_lock(&wAreaLock);
            if (c->waitingCurrent==0);
            else {
                c->waitingCurrent -= 1;
                printf("Customer %d collects a token from the gatekeeper.\n", selfIdx);
                printf("Customer %d enters the barbershop.\n", selfIdx);
                printf("Customer %d is standing in the waiting area.\n", selfIdx);
                pthread_mutex_unlock(&wAreaLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 1;
                break;
            }
            pthread_mutex_unlock(&wAreaLock);
        }
    }
    return;
}

/* Occupy a location on the waiting room sofa 
as per availability. */
void sitOnSofa(customerStruct *c, int selfIdx){
  
    if (getIdx(c->gk->customers, c->gk->custNo, pthread_self())==0){
        
        while(true){
            pthread_mutex_lock(&sofaLock);
            if (c->sofaCurrent==0);
            else {
                pthread_mutex_lock(&wAreaLock);
                c->waitingCurrent += 1;
                pthread_mutex_unlock(&wAreaLock);
                c->sofaCurrent -= 1;
                pthread_mutex_unlock(&sofaLock);
                printf("Customer %d sits in the waiting room.\n", selfIdx);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 2;
                break;
            }
            pthread_mutex_unlock(&sofaLock);
        }
    }
    else { 
        // to prevent starvation
        while(c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())-1]<2);
        while(true){
            pthread_mutex_lock(&sofaLock);
            if (c->sofaCurrent==0);
            else {
                pthread_mutex_lock(&wAreaLock);
                c->waitingCurrent += 1;
                pthread_mutex_unlock(&wAreaLock);
                c->sofaCurrent -= 1;
                pthread_mutex_unlock(&sofaLock);
                printf("Customer %d sits in the waiting room.\n", selfIdx);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 2;
                break;
            }
            pthread_mutex_unlock(&sofaLock);
        }
    }
    return;
}

/* Occupy a barber chair as per availability. */
void sitInBarberChair(customerStruct* c, int selfIdx){
    
    if (getIdx(c->gk->customers, c->gk->custNo, pthread_self())==0){
        while(true){
            pthread_mutex_lock(&barberLock);
            if (c->barberCurrent==0);
            else {
                int barberIdx = -1;
                pthread_mutex_lock(&sofaLock);
                c->sofaCurrent += 1;
                pthread_mutex_unlock(&sofaLock);
                c->barberCurrent -= 1;
                for(int i=0;i<c->gk->bNo;i++){
                    if(c->b->bChairs[i]==-1){
                        //occupy the chair
                        c->b->bChairs[i] = pthread_self();
                        barberIdx = i;
                        // wake up the barber
                        printf("Customer %d sits in barber chair %d for a haircut.\n", selfIdx, barberIdx);
                        usleep(0.01*1000000);
                        pthread_kill(c->gk->barbers[barberIdx], WAKEUP);
                        break;
                    }
                }
                pthread_mutex_unlock(&barberLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 3;
                break;
            }
            pthread_mutex_unlock(&barberLock);
        }
    }
    else { 
        // to prevent starvation
        while(c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())-1]<3);
        while(true){
            pthread_mutex_lock(&barberLock);
            if (c->barberCurrent==0);
            else {
                int barberIdx = -1;
                pthread_mutex_lock(&sofaLock);
                c->sofaCurrent += 1;
                pthread_mutex_unlock(&sofaLock);
                c->barberCurrent -= 1;
                for(int i=0;i<c->gk->bNo;i++){
                    if(c->b->bChairs[i]==-1){
                        c->b->bChairs[i] = pthread_self();
                        barberIdx = i;
                        printf("Customer %d sits in barber chair %d for a haircut.\n", selfIdx, barberIdx);
                        usleep(0.01*1000000);
                        pthread_kill(c->gk->barbers[barberIdx], WAKEUP);
                        break;
                    }
                }
                if (barberIdx==-1){
                    pthread_mutex_unlock(&barberLock);
                    continue;
                }
                pthread_mutex_unlock(&barberLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 3;
                break;
            }
            pthread_mutex_unlock(&barberLock);
        }
    }
    return;
}

/* Wait till barber is accepting payment. */
void waitForPayment(customerStruct *c, int selfIdx){
    int barberIdx = -1;
    for (int i=0;i<c->gk->bNo;i++){
    
        if (c->b->bChairs[i]==pthread_self()){
            barberIdx = i;
            break;
        }
    }
    // printf("Customer %d is waiting to pay to barber %d.\n", selfIdx, barberIdx);
    while(c->b->acceptingPayment[barberIdx]!=1){
        // wait till the barber who served you has cleaned the chair 
        // and is accepting payment (has to see if the cash register is free)
    }; // till acceptingPayment[barberIdx] is not 1. wait.
    // printf("Customer %d will pay now.\n", selfIdx);
    c->b->acceptingPayment[barberIdx] = false;
    printf("Customer %d leaves barber chair %d to pay.\n", selfIdx, barberIdx);
    c->b->gotPayment[barberIdx] = true;
    return;
}

/* Pay the barber. */
void pay(customerStruct *c, int selfIdx){
    // printf("Customer %d completes payment.\n", selfIdx);
    int barberIdx = -1;
    for (int i=0;i<c->gk->bNo;i++){
        if (c->b->bChairs[i]==pthread_self()){
            barberIdx = i;
            break;
        }
    }
    // make the payment
    c->b->amountEarned[barberIdx] += 1;
    // c->paymentDone[barberIdx] = 1;
    return;
}

/* Leave the barber chair and give your token to
the gatekeeper. */
void exitShop(customerStruct *c, int selfIdx){
    
    int barberIdx = -1;
    for (int i=0;i<c->gk->bNo;i++){
        if (c->b->bChairs[i]==pthread_self()){
            barberIdx = i;
            break;
        }
    }

    // return the token
    pthread_mutex_lock(&tokenLock);
    *c->gk->tokens += 1;
    printf("Customer %d submits the token to the gatekeeper.\n", selfIdx);
    printf("Customer %d exits the barbershop after paying for the service.\n", selfIdx);
    pthread_mutex_unlock(&tokenLock);
    
    return;
}
/* Simply say that you've left the shop. */
void leaveShop(customerStruct* c, int selfIdx){
    c->gk->customerServed[selfIdx] = true;
    printf("Customer %d goes away.\n", selfIdx);
    return;
}

/* Cut the waking thread's hair. */
void cutHair(barberStruct* b, int selfIdx, int custIdx){
    printf("Barber %d starts haircut of customer %d.\n", selfIdx, custIdx);
    float rand_wait_time = rand()/RAND_MAX*5; 
    usleep(rand_wait_time*1000000);
    printf("Barber %d finishes haircut of customer %d.\n", selfIdx, custIdx);
}

/* Check if the cleaning kit is available.
If it is, clean the chair 
else, wait for the cleaning kit. */
void cleanChair(barberStruct* b, int selfIdx, int custIdx){
    pthread_mutex_lock(&cleaningLock);
    printf("Barber %d is cleaning.\n", selfIdx);
    float rand_wait_time = (float)rand()/RAND_MAX *5;
    // printf("Wait time: %.2f.\n", rand_wait_time); 
    usleep(rand_wait_time*1000000);
    pthread_mutex_unlock(&cleaningLock);
}

/* Receive payment from the customer. See if the cashier 
is busy. If not, make the payment.*/
void acceptPayment(barberStruct* b, int selfIdx, int custIdx){
    // customer pays.
    pthread_mutex_lock(&cashierLock);
    
    // tell the customer to pay, cash register access has
    // been obtained
    b->acceptingPayment[selfIdx] = true;

    // wait till customer pays
    while(b->gotPayment[selfIdx] == false);
    
    printf("Barber %d receives payment from customer %d.\n", selfIdx, custIdx);
    // customer has payed, for future re-use set, 
    // gotPayment to false again.
    b->gotPayment[selfIdx] = false;
    
    // wait for a random amount of time to do the payment
    float rand_wait_time = (float)rand()/RAND_MAX * 3;
    // printf("Wait time: %.2f.\n", rand_wait_time); 
    usleep(rand_wait_time*1000000);
    printf("Cashier receives payment from customer %d.\n", custIdx);
    pthread_mutex_unlock(&cashierLock);
    return;
}

void wakeUpCustomer(int c){}
void wakeUpBarber(int c){}

void* customer(customerStruct* c){
    /* Assuming that all the customers whose threads are 
    created, are already in the queue waiting. Hence, 
    the pause function being called. */
    pthread_detach(pthread_self());
    int selfIdx = getIdx(c->gk->customers, c->gk->custNo, pthread_self());
    printf("Customer %d arrives at the barbershop.\n", selfIdx);
    signal(WAKEUP, wakeUpCustomer);
    pause();
    enterShop(c, selfIdx);
    sitOnSofa(c, selfIdx);
    sitInBarberChair(c, selfIdx);
    waitForPayment(c, selfIdx);
    pay(c, selfIdx);
    exitShop(c, selfIdx);
    leaveShop(c, selfIdx);
    while(true);    // preventing reassigning the same thread number
}

void* barber(customerStruct *c){
    pthread_detach(pthread_self());
    signal(WAKEUP, wakeUpBarber);
    int selfIdx = getIdx(c->gk->barbers, c->gk->bNo, pthread_self());
    while(c->b->gk->everyoneServed == false) /*put exit condition given by the gatekeeper into this while*/
    { 
        printf("Barber %d is sleeping, waiting for customers.\n", selfIdx);
        pause();
        if (c->b->gk->everyoneServed == true)
            break;
        int custIdx = getIdx(c->b->gk->customers, c->b->gk->custNo, c->b->bChairs[selfIdx]);
        // printf("Barber %d was woken up by customer %d.\n", selfIdx, custIdx);
        cutHair(c->b, selfIdx, custIdx);
        cleanChair(c->b, selfIdx, custIdx);
        acceptPayment(c->b, selfIdx, custIdx);
        
        // makes itself available
        pthread_mutex_lock(&barberLock);
        c->barberCurrent += 1;
        pthread_mutex_unlock(&barberLock);
        c->b->bChairs[selfIdx] = -1;
    }
    printf("Barber %d is leaving the shop for the day.\n", selfIdx);
}

/* Has to check for available tokens and accordingly, 
just wake up a customer process and decrement the tokens. */
void *gatekeeper(gkStruct* gk){
    pthread_detach(pthread_self());
    while(gk->custWoken<gk->custNo){
        for(int i=0;i<gk->custNo;i++){
            if (gk->customers[i]!=-1 && gk->woken[i]==false){
                float rand_wait_time = (float)rand()/RAND_MAX;
                usleep(rand_wait_time*1000000);
                // obtain lock on the tokens and do the operation.
                pthread_mutex_lock(&tokenLock);
                if (*gk->tokens>0){
                    pthread_kill(gk->customers[i], WAKEUP);
                    *gk->tokens -= 1;
                }
                else
                    // come again to check for the same customer 
                    // who couldn't be admitted this time as the 
                    // tokens were not there
                    i--;
                pthread_mutex_unlock(&tokenLock);
                gk->woken[i] = true;
                gk->custWoken += 1;
            }
        }
    }
    // once all the customers have entered, gatekeeper is done.

    // check whether all the customers are served.
    // if yes then tell the barbers to stop waiting and go home.
    for (int i=0;i<gk->custNo;i++){
        while (gk->customerServed[i]!=1){
            // wait till ith customer is served
        };
    }
    printf("All the customers are served.\n");
    gk->everyoneServed = true;  // this tells all barbers to go home.
    
    // wake up all sleeping barbers
    for(int i=0;i<gk->bNo;i++)
    pthread_kill(gk->barbers[i], WAKEUP);
    sleep(1);
    exit(0);
}



int main(int argc, char *argv[])
{
    if (argc<4)
    {
        fprintf(stderr, "Please pass all the required parameters.\n");
        exit(1);
    }

    pthread_t *barbers;
    pthread_t *bChairs;
    int bNo=0, cNo=0, wNo=0;
    
    for(int i=1;i<argc;i++){
        if (strcmp(argv[i], "-b")==0){
            bNo = atoi(argv[i+1]);
            barbers = (pthread_t*)malloc(bNo*sizeof(pthread_t));
            bChairs = (pthread_t*)malloc(bNo*sizeof(pthread_t));
        }
        if (strcmp(argv[i], "-c")==0){
            cNo = atoi(argv[i+1]);
        }
        if (strcmp(argv[i], "-w")==0){
            wNo = atoi(argv[i+1]);
        }
    }
    // wNo -= cNo;
    int MAX_CUSTOMERS = wNo + bNo + cNo;

    // the data that the gatekeeper needs access to
    gkStruct *gatekeeperData = (gkStruct*)malloc(sizeof(gkStruct));
    gatekeeperData->custNo = CUSTOMERS;
    gatekeeperData->customers = (pthread_t*)malloc(gatekeeperData->custNo*sizeof(pthread_t));
    gatekeeperData->tokens = (int*)malloc(sizeof(int));
    *(gatekeeperData->tokens) = MAX_CUSTOMERS;
    gatekeeperData->everyoneServed = false;
    gatekeeperData->customerServed = (int*)malloc(gatekeeperData->custNo*sizeof(int));
    gatekeeperData->barbers = barbers;
    gatekeeperData->bNo = bNo;
    gatekeeperData->custWoken = 0;
    gatekeeperData->woken = (int*)malloc(gatekeeperData->custNo*sizeof(int));
    for (int i=0;i<gatekeeperData->custNo;i++){
        gatekeeperData->woken[i] = false;
        gatekeeperData->customers[i] = -1;
    }

    barberStruct *barberData = (barberStruct*)malloc(sizeof(barberStruct));
    barberData->bChairs = bChairs;
    barberData->gk = gatekeeperData;
    barberData->acceptingPayment = (int*)malloc(barberData->gk->bNo*sizeof(int));
    barberData->gotPayment = (int*)malloc(barberData->gk->bNo*sizeof(int));
    barberData->amountEarned = (float*)malloc(barberData->gk->bNo*sizeof(float));
    for(int i=0;i<barberData->gk->bNo;i++)
    {
        barberData->gotPayment[i] = false;
        barberData->acceptingPayment[i] = false;
        barberData->amountEarned[i] = 0;
    }

    customerStruct *customerData = (customerStruct*)malloc(sizeof(customerStruct));
    customerData->b = (barberStruct*)malloc(sizeof(barberStruct));
    customerData->b = barberData;
    customerData->waitingCurrent = wNo;
    customerData->sofaCurrent = cNo;
    customerData->barberCurrent = bNo;
    customerData->gk = gatekeeperData;
    customerData->stages = (int*)malloc(customerData->gk->custNo*sizeof(int));

    for(int i=0;i<bNo;i++){
        // create the barber threads
        bChairs[i] = -1;        // initialize the empty chairs
        int bCheck = pthread_create(&(barbers[i]), NULL, (void *)barber, customerData);
        if (bCheck)
        {
            fprintf(stderr, "\nERROR %d: Failed while tring to create a barber thread.\n", bCheck);
            exit(1);
        }
        usleep(0.1*1000000);
    }


    // create the gatekeeper thread
    pthread_t gkID;
    int gCheck = pthread_create(&gkID, NULL, (void *)gatekeeper, gatekeeperData);
    if (gCheck){
        fprintf(stderr, "\nERROR %d: Failed while tring to create the gatekeeper thread.\n", gCheck);
        exit(1);
    }

    // creating the customers
    for(int i=0;i<CUSTOMERS;i++){
        
        float rand_wait_time = (float)rand()/RAND_MAX * 2;
        usleep(rand_wait_time*1000000);
        int cCheck = pthread_create(&(gatekeeperData->customers[i]), NULL, (void* )customer, customerData);
        if (cCheck){
            fprintf(stderr, "\nERROR %d: Failed while tring to create a customer thread.\n", cCheck);
            exit(1);
        }
    }

    pthread_exit(NULL);
}