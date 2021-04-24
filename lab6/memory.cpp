#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <queue>
#include <list>

// #define DEBUG_LRU
// #define DEBUG_FIFO
#define DEBUG_OPTIMAL

#define MAX_INPUT_SIZE 100
#define ENOUGH_SIZE 50
#define MIN(a, b, c) (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c)
using namespace std;

int getToken(char *source, char *tok, int *loc)
{
    int currSpaceNum = 0;
    int count = 0;

    for (int i = 0;; i++)
    {
        if (source[i] != ' ' && source[i] != 0)
        {
            if (currSpaceNum == *loc)
            {
                // printf("adding %c\n", source[i]);
                tok[count] = source[i];
                count += 1;
            }
            else
            {
                // printf("ignoring %c\n", source[i]);
            }
        }
        else if (source[i] == ' ')
        {
            currSpaceNum += 1;
            // printf("got space\n");
            if (count > 0)
            {
                // printf("token filled, returning\n");
                *loc += 1;
                tok[count] = '\0';
                return 0;
            }
        }
        else if (source[i] == '\0')
        {
            // printf("got null\n");
            if (count > 0)
            {
                // printf("token filled, returning\n");
                *loc += 1;
                tok[count] = '\0';
                return 0;
            }
            return -1;
        }
        // if (i==50)break;
    }
    return -1;
}

bool isPresent(int *arr, int f, int n)
{
    for (int i = 0; i < f; i++)
    {
        if (arr[i] == -1)
            break;
        else if (arr[i] == n)
            return true;
    }
    return false;
}

void replace(int arr[], int f, int src, int dst)
{

    for (int i = 0; i < f; i++)
    {
        if (arr[i] == src)
        {
            arr[i] = dst;
            return;
        }
    }
    printf("%d is not present and you are trying to replace it with %d.\n", src, dst);
    exit(1);
}

int FIFO(int arr[], int size, int f)
{

    int faults = 0;

    int pageTable[f];
    for (int i = 0; i < f; i++)
        pageTable[i] = -1;

    int curr = 0;
    queue<int> q;

    for (int i = 0; i < size; i++)
    {
        int page = arr[i];
        // check if the pageTable already has the page
        if (isPresent(pageTable, f, page) == false)
        {
            // if the page is not present it means fault has occurred
            faults += 1;

            if (curr < f)
            {
                // there is some empty space in the pageTable
                pageTable[curr] = page;
                curr += 1;

                q.push(page);

#ifdef DEBUG_FIFO
                printf("adding %d to the frames.\n", page);
#endif
            }

            else
            {
                // pageTable is full, so remove a page to add another one
                // which page to remove
                // decided using a queue
                int victim = q.front();
                replace(pageTable, f, victim, page);

                q.pop();
                q.push(page);

#ifdef DEBUG_FIFO
                printf("removing %d from the frames.\n", victim);
                printf("adding %d to the frames.\n", page);
#endif
            }
        }
    }
    return faults;
}

int LRU(int arr[], int size, int f)
{

    int faults = 0;

    int pageTable[f];
    for (int i = 0; i < f; i++)
        pageTable[i] = -1;

    int curr = 0;

    list<int> l;

    for (int i = 0; i < size; i++)
    {
        int page = arr[i];
        // check if the pageTable already has the page
        if (isPresent(pageTable, f, page) == false)
        {
            // if the page is not present it means fault has occurred
            faults += 1;

            if (curr < f)
            {
                // there is some empty space in the pageTable
                pageTable[curr] = page;
                curr += 1;
                l.push_front(page);
#ifdef DEBUG_LRU
                printf("adding %d to the frames.\n", page);
#endif
            }

            else
            {
                // pageTable is full, so remove a page to add another one
                // which page to remove
                // decided using a queue
                int victim = l.back();
                l.pop_back();
                l.push_front(page);
                replace(pageTable, f, victim, page);
#ifdef DEBUG_LRU
                printf("removing %d from the frames.\n", victim);
                printf("adding %d to the frames.\n", page);
#endif
            }
        }
        else
        {
            // the page is already present in the pageTable
            // however, need to change it's last accessed time
            // push it to the front of the list
            l.remove(page);
            l.push_front(page);
#ifdef DEBUG_LRU
            printf("%d is already present. Changing its most recent use.\n", page);
#endif
        }
    }

    return faults;
}

int getVictim(int pageTable[], int f, int arr[], int curr, int size)
{
    int firstUse[f];
    for(int i=0;i<f;i++)
    firstUse[i] = -1;
    for (int i = 0; i < f; i++)
    {
        int page = pageTable[i];
        for (int j = curr; j < size; j++)
        {
            if (arr[j] == page)
            {
                firstUse[i] = j;
                break;
            }
        }
    }

    int largest = -1;
    for(int i=0;i<f;i++){
        if (firstUse[i]==-1){
            largest = i;
            break;
        }
        else if (firstUse[i]>largest)
        largest = i;
    }
    return pageTable[largest];
}

int OPTIMAL(int arr[], int size, int f)
{

    int faults = 0;

    int pageTable[f];
    for (int i = 0; i < f; i++)
        pageTable[i] = -1;

    int curr = 0;

    for (int i = 0; i < size; i++)
    {
        int page = arr[i];
        // check if the pageTable already has the page
        if (isPresent(pageTable, f, page) == false)
        {
            // if the page is not present it means fault has occurred
            faults += 1;

            if (curr < f)
            {
                // there is some empty space in the pageTable
                pageTable[curr] = page;
                curr += 1;

#ifdef DEBUG_OPTIMAL
                printf("adding %d to the frames.\n", page);
#endif
            }

            else
            {
                // pageTable is full, so remove a page to add another one
                // which page to remove
                // decided using a queue
                int victim = getVictim(pageTable, f, arr, i + 1, size);

                replace(pageTable, f, victim, page);

#ifdef DEBUG_OPTIMAL
                printf("removing %d from the frames.\n", victim);
                printf("adding %d to the frames.\n", page);
#endif
            }
        }
    }

    return faults;
}

int main(int argc, char *argv[])
{
    int n = -1;
    if (argc < 2)
    {
        printf("Enter the number of iterations as a parameter.\n");
        exit(1);
    }

    // number of iterations to go for
    n = atoi(argv[1]);

    printf("Enter the number of frames allocated to the process: ");
    int f = -1;
    scanf("%d", &f);
    getchar();

    for (int i = 0; i < n; i++)
    {
        printf("Enter the reference string: ");
        char refString[MAX_INPUT_SIZE];
        fscanf(stdin, "%[^\n]s", refString);
        getchar();

        int size = 0;
        int counter = 0;
        char tmp[5];
        while (getToken(refString, tmp, &counter) != -1)
            size += 1;

        int ref[size];
        int curr = 0;
        counter = 0;
        while (getToken(refString, tmp, &counter) != -1)
            ref[curr++] = atoi(tmp);

        int fifoFaults = FIFO(ref, size, f);
        int lruFaults = LRU(ref, size, f);
        int optimalFaults = OPTIMAL(ref, size, f);

        printf("Faults:\n");
        printf("FIFO: %d LRU: %d OPTIMAL: %d\n", fifoFaults, lruFaults, optimalFaults);

        if (fifoFaults <= lruFaults)
        {
            if (fifoFaults <= optimalFaults)
            {
                printf("The best is FIFO.\n");
            }
            else
            {
                printf("The best is OPTIMAL.\n");
            }
        }
        else
        {
            if (lruFaults <= optimalFaults)
            {
                printf("The best is LRU.\n");
            }
            else
            {
                printf("The best is OPTIMAL.\n");
            }
        }
        exit(4);

        printf("Checking for Belady's Anomaly...\n");
        int prevBest = fifoFaults;
        bool beladys = false;
        for (int k = f + 1; k < f + 10; k++)
        {
            fifoFaults = FIFO(ref, size, k);
            if (fifoFaults < prevBest)
            {
                prevBest = fifoFaults;
            }
            else
            {
                // there was an increase in the number of faults
                // i.e. BELADY'S ANOMALY
                printf("Belady's anomaly occurred!\n");
                printf("There was increase in the number of page faults when frame number was increased from %d to %d\n", k - 1, k);
                beladys = true;
                break;
            }
        }
        if (beladys == false)
        {
            printf("Belady's Anomaly wasn't encountered.\n");
            printf("Number of frames was varied from %d to %d.\n", f + 1, f + 10);
        }
    }
}