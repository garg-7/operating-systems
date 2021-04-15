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

pthread_mutex_t tokenLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wAreaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sofaLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct customerStruct{
    pthread_t *bChairs;
    pthread_t *wArea;
    pthread_t *wChairs;
    int bNo;
    int wNo;
    int cNo;
    int nextSofa;
    int nextWaiting;
    int nextBarber;
    int *tokens;
} customerStruct;

typedef struct barberStruct{
    pthread_t cleaningKit;          // the barber that is using the cleaning kit
} barberStruct;

typedef struct gkStuct{
    int *tokens;
    pthread_t *customers;
    int cNo;
} gkStruct;

void enterShop(customerStruct *c){
    /* Gatekeeper has given you the token.
    Occupy a seat in the waiting area */
    pthread_mutex_lock(&wAreaLock);
    c->wArea[c->nextWaiting] = pthread_self();
    c->nextWaiting =+ 1;
    c->nextWaiting = (c->nextWaiting) % (c-> wNo);
    pthread_mutex_unlock(&wAreaLock);
    return;
}

void sitOnSofa(customerStruct *c){
    /* Occupy a location in the waiting chairs 
    as per availability. */
    while(true){
        pthread_mutex_lock(&sofaLock);
        if (c->nextSofa==-1);
        else {

            c->wArea[c->nextWaiting] = pthread_self();
            c->nextWaiting =+ 1;
            c->nextWaiting = (c->nextWaiting) % (c-> wNo);
            break;
        }
        pthread_mutex_unlock(&sofaLock);
    }
}

void sitInBarberChair(){
    /* Occupy a barber chair as per availability. */
}

void waitForPayment(){
    /* Wait till barber is accepting payment. */
}

void pay(){
    /* Pay the barber. */
}

void exitShop(){
    /* Leave the barber chair and give your token to
    the gatekeeper. */
}
void leaveShop(){
    /* Simply say that you've left the shop. */
}

void cutHair(){
    /* Cut the waking thread's hair. */
}

void cleanChair(){
    /* Check if the cleaning kit is available.
    If it is, clean the chair and ask for payment from 
        the customer.
    else, wait for the cleaning kit. */
}

void acceptPayment(){
    /* Receive payment from the customer. See if the cashier 
    is busy. If not, make the payment and sleep again.*/
}

void* customer(customerStruct* c){
    /* Assuming that all the customers whose threads are 
    created, are already in the queue waiting. Hence, 
    the pause function being called. */
    pause();
    printf("Thread %ld entered the shop.\n", pthread_self());
    enterShop(c);
    sitOnSofa(c);
    sitInBarberChair(c);
    waitForPayment(c);
    pay(c);
    exitShop(c); // incrementing the tokens
    leaveShop(c);
}

void* barber(barberStruct *b){

    while(true) /*put exit condition given by the gatkeeper into this while*/
    { 
        pause();
        cutHair();
        cleanChair();
        acceptPayment();
    }
}

/* Has to check for available tokens and accordingly, 
just wake up a customer process and decrement the tokens. */
void *gatekeeper(gkStruct* gk){
    
    for(int i=0;i<gk->cNo;i++){
        
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
    gatekeeperData->cNo = CUSTOMERS;
    gatekeeperData->customers = (pthread_t*)malloc(gatekeeperData->cNo*sizeof(pthread_t));
    gatekeeperData->tokens = (int*)malloc(sizeof(int));
    *(gatekeeperData->tokens) = MAX_CUSTOMERS;

    customerStruct *customerData = (customerStruct*)malloc(sizeof(customerStruct));
    customerData->tokens = gatekeeperData->tokens;
    customerData->bChairs = bChairs;
    customerData->wArea = wArea;
    customerData->wChairs = wChairs;
    customerData->wNo = wNo;
    customerData->bNo = bNo;
    customerData->cNo = cNo;
    customerData->nextWaiting = 0;
    customerData->nextSofa = 0;
    customerData->nextBarber = 0;

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