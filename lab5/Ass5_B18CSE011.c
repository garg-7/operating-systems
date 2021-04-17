#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>

#define CUSTOMERS 6
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
pthread_mutex_t cleaningLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cashierLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct gkStuct{
    pthread_t *customers;
    pthread_t *barbers;
    int bNo;
    int *tokens;
    int custNo;
    int *customerServed;
    int everyoneServed;

} gkStruct;

typedef struct barberStruct{
    pthread_t *barbers;
    pthread_t *bChairs;
    gkStruct* gk;
    int *acceptingPayment;
    int* gotPayment;
    float *amountEarned;
    int bNo;
} barberStruct;

typedef struct customerStruct{
    gkStruct* gk;
    barberStruct* b;
    int bNo;
    int wNo;
    int cNo;
    int sofaCurrent;
    int waitingCurrent;
    int barberCurrent;
    int *acceptingPayment;
    // int *paymentDone;
    int *stages;
} customerStruct;



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
    if (getIdx(c->gk->customers, c->gk->custNo, pthread_self())==0){
        // printf("0th customer.\n");
        while(true){
            pthread_mutex_lock(&wAreaLock);
            if (c->waitingCurrent==0);
            else {
                c->waitingCurrent -= 1;
                printf("Customer %d collects a token from the gatekeeper.\n", selfIdx);
                printf("Customer %d enters the barbershop.\n", selfIdx);
                printf("Customer %d is in the waiting area.\n", selfIdx);
                pthread_mutex_unlock(&wAreaLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 1;
                break;
            }
            pthread_mutex_unlock(&wAreaLock);
        }
    }
    else { // to prevent starvation
        while(c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())-1]<1);
        while(true){
            pthread_mutex_lock(&wAreaLock);
            if (c->waitingCurrent==0);
            else {
                c->waitingCurrent -= 1;
                printf("Customer %d collects a token from the gatekeeper.\n", selfIdx);
                printf("Customer %d enters the barbershop.\n", selfIdx);
                printf("Customer %d is in the waiting area.\n", selfIdx);
                pthread_mutex_unlock(&wAreaLock);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 1;
                break;
            }
            pthread_mutex_unlock(&wAreaLock);
        }
    }
    return;
}

void sitOnSofa(customerStruct *c, int selfIdx){
    /* Occupy a location in the waiting chairs 
    as per availability. */
    // printf("here5\n");
    if (getIdx(c->gk->customers, c->gk->custNo, pthread_self())==0){
        // printf("0th customer.\n");
        while(true){
            pthread_mutex_lock(&sofaLock);
            if (c->sofaCurrent==0);
            else {
                pthread_mutex_lock(&wAreaLock);
                c->waitingCurrent += 1;
                pthread_mutex_unlock(&wAreaLock);
                c->sofaCurrent -= 1;
                pthread_mutex_unlock(&sofaLock);
                printf("Customer %d takes a seat on the sofa in the waiting room.\n", selfIdx);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 2;
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
                printf("Customer %d takes a seat on the sofa in the waiting room.\n", selfIdx);
                c->stages[getIdx(c->gk->customers, c->gk->custNo, pthread_self())] = 2;
                break;
            }
            pthread_mutex_unlock(&sofaLock);
        }
    }
    return;
}

void sitInBarberChair(customerStruct* c, int selfIdx){
    /* Occupy a barber chair as per availability. */
    // usleep(0.1*1000000);
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
                    if(c->b->bChairs[i]==-1){
                        //occupy the chair
                        c->b->bChairs[i] = pthread_self();
                        barberIdx = i;
                        // wake up the barber
                        printf("Customer %d sits on barber chair %d for a haircut.\n", selfIdx, barberIdx);
                        usleep(0.01*1000000);
                        pthread_kill(c->b->barbers[barberIdx], WAKEUP);
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
                    if(c->b->bChairs[i]==-1){
                        c->b->bChairs[i] = pthread_self();
                        barberIdx = i;
                        printf("Customer %d sits on barber chair %d for a haircut.\n", selfIdx, barberIdx);
                        usleep(0.01*1000000);
                        pthread_kill(c->b->barbers[barberIdx], WAKEUP);
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

void waitForPayment(customerStruct *c, int selfIdx){
    /* Wait till barber is accepting payment. */
    int barberIdx = -1;
    for (int i=0;i<c->bNo;i++){
        // printf("currinchair: %d. for self %d\n", c->b->bChairs[i], selfIdx);
        if (c->b->bChairs[i]==pthread_self()){
            barberIdx = i;
            break;
        }
    }
    printf("Customer %d is waiting to pay to barber %d.\n", selfIdx, barberIdx);
    while(c->acceptingPayment[barberIdx]!=1){
        // printf("%d.\n", c->acceptingPayment[barberIdx]);
        // sleep(3);
    }; // till acceptingPayment[barberIdx] is not 1. wait.
    printf("Customer %d will pay now.\n", selfIdx);
    c->acceptingPayment[barberIdx] = 0;
    c->b->gotPayment[barberIdx] = 1;
    return;
}

void pay(customerStruct *c, int selfIdx){
    /* Pay the barber. */
    printf("Customer %d completes payment.\n", selfIdx);
    int barberIdx = -1;
    for (int i=0;i<c->bNo;i++){
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

void exitShop(customerStruct *c, int selfIdx){
    /* Leave the barber chair and give your token to
    the gatekeeper. */
    
    int barberIdx = -1;
    for (int i=0;i<c->bNo;i++){
        if (c->b->bChairs[i]==pthread_self()){
            barberIdx = i;
            break;
        }
    }

    // // leave the chair
    // c->b->bChairs[barberIdx] = -1; 

    // // increment barber chairs
    // pthread_mutex_lock(&barberLock);
    // c->barberCurrent += 1;
    // pthread_mutex_unlock(&barberLock);

    // return the token
    pthread_mutex_lock(&tokenLock);
    *c->gk->tokens += 1;
    pthread_mutex_unlock(&tokenLock);
    
    return;
}
void leaveShop(customerStruct* c, int selfIdx){
    /* Simply say that you've left the shop. */
    
    c->gk->customerServed[selfIdx] = 1;
    printf("Customer %d has left the shop.\n", selfIdx);
    return;
}

void cutHair(barberStruct* b, int selfIdx, int custIdx){
    /* Cut the waking thread's hair. */
    printf("Barber %d started haircut of customer %d.\n", selfIdx, custIdx);
    float rand_wait_time = rand()/RAND_MAX*5; 
    usleep(rand_wait_time*1000000);
    printf("Barber %d finishes haircut of customer %d.\n", selfIdx, custIdx);
}

void cleanChair(barberStruct* b, int selfIdx, int custIdx){
    /* Check if the cleaning kit is available.
    If it is, clean the chair and ask for payment from 
        the customer.
    else, wait for the cleaning kit. */
    pthread_mutex_lock(&cleaningLock);
    printf("Barber %d is cleaning.\n", selfIdx);
    float rand_wait_time = (float)rand()/RAND_MAX *5;
    // printf("Wait time: %.2f.\n", rand_wait_time); 
    usleep(rand_wait_time*1000000);
    pthread_mutex_unlock(&cleaningLock);
}

void acceptPayment(barberStruct* b, int selfIdx, int custIdx){
    /* Receive payment from the customer. See if the cashier 
    is busy. If not, make the payment and sleep again.*/
    b->acceptingPayment[selfIdx]=1;
    // printf("%d for barber %d.\n", b->acceptingPayment[selfIdx], selfIdx);
    // customer pays.
    while(b->gotPayment[selfIdx] != 1);
    b->gotPayment[selfIdx] = 0;
    pthread_mutex_lock(&cashierLock);
    printf("Barber %d receives payment from customer %d.\n", selfIdx, custIdx);
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
    printf("Customer %d arrived at the barbershop.\n", selfIdx);
    signal(WAKEUP, wakeUpCustomer);
    pause();
    enterShop(c, selfIdx);
    sitOnSofa(c, selfIdx);
    sitInBarberChair(c, selfIdx);
    waitForPayment(c, selfIdx);
    pay(c, selfIdx);
    exitShop(c, selfIdx);
    leaveShop(c, selfIdx);
}

void* barber(customerStruct *c){
    pthread_detach(pthread_self());
    signal(WAKEUP, wakeUpBarber);
    int selfIdx = getIdx(c->b->barbers, c->b->bNo, pthread_self());
    while(c->b->gk->everyoneServed == false) /*put exit condition given by the gatekeeper into this while*/
    { 
        printf("Barber %d is waiting for customers, sleeping in a chair.\n", selfIdx);
        pause();
        if (c->b->gk->everyoneServed == true)
            break;
        int custIdx = getIdx(c->b->gk->customers, c->b->gk->custNo, c->b->bChairs[selfIdx]);
        printf("Barber %d was woken up by customer %d.\n", selfIdx, custIdx);
        cutHair(c->b, selfIdx, custIdx);
        cleanChair(c->b, selfIdx, custIdx);
        acceptPayment(c->b, selfIdx, custIdx);
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
    // printf("i'm here.\n");
    for(int i=0;i<gk->custNo;i++){
        
        // obtain lock on the tokens and do the operation.
        pthread_mutex_lock(&tokenLock);
        if (*gk->tokens>0){
            // printf("Gatekeeper allowed customer %d in.\n", i);
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
        while (gk->customerServed[i]!=1){
            // printf("Customer %d: %d.\n",i,  gk->customerServed[i]);
            // sleep(4);
        };
    }
    printf("All the customers are served.\n");
    gk->everyoneServed = true;
    // wake up all sleeping barbers
    for(int i=0;i<gk->bNo;i++)
    pthread_kill(gk->barbers[i], WAKEUP);
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
    wNo -= cNo;
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

    barberStruct *barberData = (barberStruct*)malloc(sizeof(barberStruct));
    barberData->barbers = barbers;
    barberData->bChairs = bChairs;
    barberData->gk = gatekeeperData;
    barberData->bNo = bNo;
    barberData->acceptingPayment = (int*)malloc(barberData->bNo*sizeof(int));
    barberData->gotPayment = (int*)malloc(barberData->bNo*sizeof(int));
    barberData->amountEarned = (float*)malloc(barberData->bNo*sizeof(float));
    for(int i=0;i<barberData->bNo;i++)
    {
        barberData->gotPayment[i] = 0;
        barberData->acceptingPayment[i] = 0;
        barberData->amountEarned[i] = 0;
    }

    customerStruct *customerData = (customerStruct*)malloc(sizeof(customerStruct));
    customerData->b = (barberStruct*)malloc(sizeof(barberStruct));
    customerData->b = barberData;
    customerData->acceptingPayment = (int*)malloc(barberData->bNo*sizeof(int));
    customerData->acceptingPayment = barberData->acceptingPayment;
    customerData->wNo = wNo;
    customerData->bNo = bNo;
    customerData->cNo = cNo;
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
        // bChairs[i] = barbers[i];    // put the barber in the chair
        usleep(0.1*1000000);
    }


    // customerData->paymentDone = (int*)malloc(customerData->gk->custNo*sizeof(int));

    // creating the customers
    for(int i=0;i<CUSTOMERS;i++){
        int cCheck = pthread_create(&(gatekeeperData->customers[i]), NULL, (void* )customer, customerData);
        if (cCheck){
            fprintf(stderr, "\nERROR %d: Failed while tring to create the gatekeeper thread.\n", cCheck);
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
    pthread_exit(NULL);
}