#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CUSTOMERS 20
#define CUSTOMERS 50

void enterShop(){
    /* Check if gatekeeper has token. 
    If yes, take 1 and enter i.e.
        occupy a location in the waiting area.
    Else wait in a queue.*/
}

void sitOnSofa(){
    /* Occupy a location in the waiting chairs 
    as per availability. */
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
    pause();
    enterShop();
    sitOnSofa();
    sitInBarberChair();
    waitForPayment();
    pay();
    exitShop();
    leaveShop();
}

void* barber(barberStruct *b){

    while(true) /*put exit condition given by the gatkeeper into this while*/
    { 
        pause();
        cutHair()
        cleanChair();
        acceptPayment();
    }
}

void *gatekeeper(gkStruct* gk){

}

typedef struct customerStruct{
    pthread_t *bChairs;
    pthread_t *wArea;
    pthread_t *wChairs;
    
} customerStruct;

typedef struct barberStruct{
    pthread_t cleaningKit;          // the barber that is using the cleaning kit
} barberStruct;

typedef struct gkStuct{
    int tokens[MAX_CUSTOMERS];
    pthread_t *customers;
} gkStruct;


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
    gatekeeperData->customers = (pthread_t*)malloc(CUSTOMERS*sizeof(pthread_t));
    
    customerStruct *customerData = (customerStruct*)malloc(sizeof(customerStruct));
    // creating the customers
    for(int i=0;i<CUSTOMERS;i++){
        int cCheck = pthread_create(&(gatekeeperData->customers[i]), NULL, (void* )customer, &customerData);
        if (cCheck){
            fprintf(stderr, "\nERROR %d: Failed while tring to create the gatekeeper thread.\n", cCheck);
            exit(1);
        }
    }

    // create the gatekeeper thread
    pthread_t gkID;
    int gCheck = pthread_create(&gkID, NULL, (void *)gatekeeper, &gatekeeperData);
    if (gCheck){
        fprintf(stderr, "\nERROR %d: Failed while tring to create the gatekeeper thread.\n", gCheck);
        exit(1);
    }




}