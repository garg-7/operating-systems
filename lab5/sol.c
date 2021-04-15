#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#define CUSTOMERS 50
#define WAKEUP SIGUSR1
#define SLEEPNOW SIGUSR2
#define STAGE1
#define STAGE2
#define STAGE3
#define STAGE4
#define STAGE5

pthread_mutex_t tokenLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wAreaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sofaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t barberLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct gkStuct{
    pthread_t *customers;
    int *tokens;
    int custNo;
    int *customerServed;
    int everyoneServed;
} gkStruct;

typedef struct customerStruct{
    pthread_t *bChairs;
    pthread_t * barbers;
    gkStruct* gk;
    int bNo;
    int wNo;
    int cNo;
    int sofaCurrent;
    int waitingCurrent;
    int barberCurrent;
    int *acceptingPayment;
    int *paymentDone;
    int *stages;
    float *amountEarned;
} customerStruct;

typedef struct barberStruct{
    pthread_t *cleaningKit;          // the barber that is using the cleaning kit
    pthread_t *cashier;              // the barber that is using the cash register
    pthread_t *barbers;
    pthread_t *bChairs;
    gkStruct* gk;
    int bNo;
} barberStruct;


int getIdx(pthread_t * c, int n, pthread_t curr){
    for(int i=0;i<n;i++){
        if (c[i]==curr)
            return i;
    }
    printf("[ERROR] Thread %ld is not in the list.\n", curr);
    exit(3);
}

void enterShop(customerStruct *c, int selfIdx){
    /* Gatekeeper has given you the token.
    Occupy a seat in the waiting area */
    printf("Customer %d collects a token from the gatekeeper.\n", selfIdx);
    pthread_mutex_lock(&wAreaLock);
    c->waitingCurrent --;
    pthread_mutex_unlock(&wAreaLock);
    c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 1;
    printf("Customer %d enters the barbershop.\n", selfIdx);
    printf("Customer %d is in the waiting area.\n", selfIdx);
    return;
}

void sitOnSofa(customerStruct *c, int selfIdx){
    /* Occupy a location in the waiting chairs 
    as per availability. */
    
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
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 2;
                printf("Customer %d takes a seat on the sofa in the waiting room.\n", selfIdx);
                break;
            }
            pthread_mutex_unlock(&sofaLock);
        }
    }
    else { // to prevent starvation
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
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 2;
                printf("Customer %d takes a seat on the sofa in the waiting room.\n", selfIdx);
                break;
            }
            pthread_mutex_unlock(&sofaLock);
        }
    }
    return;
}

void sitInBarberChair(customerStruct* c, int selfIdx){
    /* Occupy a barber chair as per availability. */
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
                for(int i=0;i<c->bNo;i++){
                    if(c->bChairs[i]==-1){
                        //occupy the chair
                        c->bChairs[i] = pthread_self();
                        barberIdx = i;
                        // wake up the barber
                        pthread_kill(c->barbers[barberIdx], WAKEUP);
                        break;
                    }
                }
                pthread_mutex_unlock(&barberLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 3;
                printf("Customer %d sits on barber chair %d for a haircut.\n", selfIdx, barberIdx);
                break;
            }
            pthread_mutex_unlock(&barberLock);
        }
    }
    else { // to prevent starvation
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
                for(int i=0;i<c->bNo;i++){
                    if(c->bChairs[i]==-1){
                        c->bChairs[i] = pthread_self();
                        barberIdx = i;
                        break;
                    }
                }
                pthread_mutex_unlock(&barberLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 3;
                printf("Customer %d sits on barber chair %d for a haircut.\n", selfIdx, barberIdx);
                break;
            }
            pthread_mutex_unlock(&barberLock);
        }
    }
    return;
}

void waitForPayment(customerStruct *c, int selfIdx){
    /* Wait till barber is accepting payment. */
    int selfIdx = getIdx(c->gk->customers, c->gk->custNo, pthread_self());
    int barberIdx = -1;
    for (int i=0;i<c->bNo;i++){
        if (c->bChairs[i]==selfIdx){
            barberIdx = i;
            break;
        }
    }
    while(c->acceptingPayment[barberIdx]!=1); // till acceptingPayment[barberIdx] is not 1. wait.
    return;
}

void pay(customerStruct *c, int selfIdx){
    /* Pay the barber. */
    int selfIdx = getIdx(c->gk->customers, c->gk->custNo, pthread_self());
    int barberIdx = -1;
    for (int i=0;i<c->bNo;i++){
        if (c->bChairs[i]==selfIdx){
            barberIdx = i;
            break;
        }
    }
    // make the payment
    c->amountEarned[barberIdx] += 1;
    c->paymentDone[barberIdx] = 1;
    return;
}

void exitShop(customerStruct *c, int selfIdx){
    /* Leave the barber chair and give your token to
    the gatekeeper. */
    int selfIdx = getIdx(c->gk->customers, c->gk->custNo, pthread_self());
    int barberIdx = -1;
    for (int i=0;i<c->bNo;i++){
        if (c->bChairs[i]==selfIdx){
            barberIdx = i;
            break;
        }
    }
    // leave the chair
    c->bChairs[barberIdx] = -1; 

    // increment barber chairs
    pthread_mutex_lock(&barberLock);
    c->barberCurrent += 1;
    pthread_mutex_unlock(&barberLock);

    // return the token
    pthread_mutex_lock(&tokenLock);
    *c->gk->tokens += 1;
    pthread_mutex_unlock(&tokenLock);
    
    return;
}
void leaveShop(customerStruct* c, int selfIdx){
    /* Simply say that you've left the shop. */
    int selfIdx = getIdx(c->gk->customers, c->gk->custNo, pthread_self());
    c->gk->customerServed[selfIdx] = 1;
    printf("Customer %d has left the shop.\n", selfIdx);
    return;
}

void cutHair(barberStruct* b, int selfIdx, int custIdx){
    /* Cut the waking thread's hair. */

}

void cleanChair(barberStruct* b, int selfIdx, int custIdx){
    /* Check if the cleaning kit is available.
    If it is, clean the chair and ask for payment from 
        the customer.
    else, wait for the cleaning kit. */
}

void acceptPayment(barberStruct* b, int selfIdx, int custIdx){
    /* Receive payment from the customer. See if the cashier 
    is busy. If not, make the payment and sleep again.*/
    
}

void* customer(customerStruct* c){
    /* Assuming that all the customers whose threads are 
    created, are already in the queue waiting. Hence, 
    the pause function being called. */
    int selfIdx = getIdx(c->gk->customers, c->gk->custNo, pthread_self());
    printf("Customer %d arrived at the barbershop.\n", selfIdx);
    
    pause();
    enterShop(c, selfIdx);
    sitOnSofa(c, selfIdx);
    sitInBarberChair(c, selfIdx);
    waitForPayment(c, selfIdx);
    pay(c, selfIdx);
    exitShop(c, selfIdx);
    leaveShop(c, selfIdx);
}

void* barber(barberStruct *b){

    int selfIdx = getIdx(b->barbers, b->bNo, pthread_self());
    while(b->gk->everyoneServed == false) /*put exit condition given by the gatekeeper into this while*/
    { 
        printf("Barber %d is waiting for customers, sleeping in a chair.\n", selfIdx);
        pause();
        int custIdx = getIdx(b->gk->customers, b->gk->custNo, b->bChairs[selfIdx]);
        printf("Barber %d was woken up by customer %d.\n", selfIdx, custIdx);
        cutHair(b, selfIdx);
        cleanChair(b, selfIdx);
        acceptPayment(b, selfIdx);
    }
}

/* Has to check for available tokens and accordingly, 
just wake up a customer process and decrement the tokens. */
void *gatekeeper(gkStruct* gk){
    
    for(int i=0;i<gk->custNo;i++){
        
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
    
    }
    // once all the customers have entered, gatekeeper is done.

    // check whether all the customers are served.
    // if yes then tell the barbers to stop waiting and go home.
    for (int i=0;i<gk->custNo;i++){
        while (gk->customerServed[i]!=1);
    }
    gk->everyoneServed = true;
    return;
}



int main(int argc, char *argv[])
{
    if (argc<4)
    {
        fprintf(stderr, "Please pass all the required parameters.\n");
        exit(1);
    }

    pthread_t *bChairs;          // which threads are in the barber chairs
    pthread_t *wChairs;          // which threads are in the waiting chairs
    pthread_t *wArea;            // which threads are in the waiting area
    pthread_t *barbers;
    int bNo=0, cNo=0, wNo=0;
    
    for(int i=1;i<argc;i++){
        if (strcmp(argv[i], "-b")==0){
            bNo = atoi(argv[i+1]);
            bChairs = (pthread_t*)malloc(bNo*sizeof(pthread_t));
            barbers = (pthread_t*)malloc(bNo*sizeof(pthread_t));
        }
        if (strcmp(argv[i], "-c")==0){
            cNo = atoi(argv[i+1]);
            wChairs = (pthread_t*)malloc(cNo*sizeof(pthread_t));
        }
        if (strcmp(argv[i], "-w")==0){
            wNo = atoi(argv[i+1]);
            wArea = (pthread_t*)malloc((wNo-cNo)*sizeof(pthread_t));      
        }
    }
    int MAX_CUSTOMERS = wNo + bNo;
    barberStruct *barberData = (barberStruct*)malloc(sizeof(barberStruct));
    barberData->barbers = barbers;
    barberData->bNo = bNo;
    barberData->cashier = (pthread_t*)malloc(sizeof(pthread_t));
    barberData->cleaningKit = (pthread_t*)malloc(sizeof(pthread_t));

    for(int i=0;i<bNo;i++){
        // create the barber threads
        bChairs[i] = -1;        // initialize the empty chairs
        
        int bCheck = pthread_create(&(barbers[i]), NULL, (void *)barber, barberData);
        if (bCheck)
        {
            fprintf(stderr, "\nERROR %d: Failed while tring to create a barber thread.\n", bCheck);
            exit(1);
        }
        bChairs[i] = barbers[i];    // put the barber in the chair
    }

    // the data that the gatekeeper needs access to
    gkStruct *gatekeeperData = (gkStruct*)malloc(sizeof(gkStruct));
    gatekeeperData->custNo = CUSTOMERS;
    gatekeeperData->customers = (pthread_t*)malloc(gatekeeperData->custNo*sizeof(pthread_t));
    gatekeeperData->tokens = (int*)malloc(sizeof(int));
    *(gatekeeperData->tokens) = MAX_CUSTOMERS;
    gatekeeperData->everyoneServed = false;
    gatekeeperData->customerServed = (int*)malloc(gatekeeperData->custNo*sizeof(int));

    customerStruct *customerData = (customerStruct*)malloc(sizeof(customerStruct));
    customerData->bChairs = bChairs;    // initialize with -1
    customerData->wNo = wNo;
    customerData->bNo = bNo;
    customerData->cNo = cNo;
    customerData->waitingCurrent = 0;
    customerData->sofaCurrent = 0;
    customerData->barberCurrent = 0;
    customerData->acceptingPayment = (int*)malloc(customerData->bNo*sizeof(int));
    customerData->gk = gatekeeperData;
    customerData->amountEarned = (float*)malloc(customerData->bNo*sizeof(float));

    // creating the customers
    for(int i=0;i<CUSTOMERS;i++){
        int cCheck = pthread_create(&(gatekeeperData->customers[i]), NULL, (void* )customer, customerData);
        if (cCheck){
            fprintf(stderr, "\nERROR %d: Failed while tring to create the gatekeeper thread.\n", cCheck);
            exit(1);
        }
    }

    // create the gatekeeper thread
    pthread_t gkID;
    int gCheck = pthread_create(&gkID, NULL, (void *)gatekeeper, gatekeeperData);
    if (gCheck){
        fprintf(stderr, "\nERROR %d: Failed while tring to create the gatekeeper thread.\n", gCheck);
        exit(1);
    }
}