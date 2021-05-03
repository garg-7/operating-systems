#include <stdio.h>
#include <stdlib.h>
#include <list>

// TLB flushed on every process load
typedef struct TLB
{
    int **store; // store[max_size][2] - <pageNumber, frameNumber>
    int max_size;
} TLB;

// separate page table for every process
typedef struct pageTable
{
    int **store; // store[max_size][2] - <frameNumber, validityBit>
    int max_size;
} pageTable;


int main()
{
    int k= -1, m = -1, f = -1, s = -1;
    printf("Enter the number of processes: ");
    scanf("%d", &k);

    printf("Enter the number of max pages needed per process: ");
    scanf("%d", &m);

    printf("Enter the number of frames in main memory: ");
    scanf("%d", &f);

    printf("Enter the size of TLB: ");
    scanf("%d", &s);

    TLB t;
    t.max_size = s;
    t.store = (int**)malloc(t.max_size*sizeof(int*));
    if (t.store==NULL)
    {
        printf("Malloc failed\n");
        exit(1);
    }
    for (int i=0;i<t.max_size;i++)
    {
        t.store[i] = (int*)malloc(2*sizeof(int));
        if (t.store[i]==NULL)
        {
            printf("Malloc failed\n");
            exit(1);
        }
    }



}