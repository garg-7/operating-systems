#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_REQUEST_SIZE 20


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

bool bankersSafety(int * resSeq, int **need, int **alloc, int avail[], int n, int m)
{
    bool done[n];
    for(int i=0;i<n;i++)
        done[i] = false;


    int curr = 0;

    bool changeHappened = true;
    while(changeHappened)
    {
        changeHappened = false;
        for(int i=0;i<n;i++)
        {
            if (done[i]==false)
            {   
                bool handle = true;
                for(int j=0;j<m;j++)
                {
                    if (need[i][j]>avail[j])
                    {
                        handle = false;
                        break;
                    }
                }
                if (handle == true)
                {
                    changeHappened = true;

                    for(int j=0;j<m;j++)
                    {
                        avail[j] += alloc[i][j];
                    }
                    done[i] = true;
                    resSeq[curr] = i;
                    curr += 1;
                }
            }
        }
    }
    bool allDone = true;
    for(int i=0;i<n;i++)
    {
        if (done[i]==false)
        {
            allDone = false;
            break;
        }
    }

    if (allDone==true)
    {
        return true;
    }

    return false;
}

int main()
{
    printf("Enter the number of processes: ");
    int n  = -1;
    scanf("%d", &n);

    printf("Enter the number of resource types: ");
    int m = -1;
    scanf("%d", &m);

    printf("Enter the allocation matrix: \n\t");
     for(int i=0;i<m;i++)
    {
        printf("R%d\t", i);
    }
    printf("\n");
    int **allocation = (int**)malloc(n*sizeof(int*));
    for(int i=0;i<n;i++)
    {
        allocation[i] = (int*)malloc(m*sizeof(int));
        printf("P%d:\t", i);
        for(int j=0;j<m;j++)
        {
            scanf("%d", &allocation[i][j]);
        }
    }

    printf("Enter the max matrix: \n\t");
     for(int i=0;i<m;i++)
    {
        printf("R%d\t", i);
    }
    printf("\n");
    int **max = (int**)malloc(n*sizeof(int*));
    for(int i=0;i<n;i++)
    {
        max[i] = (int*)malloc(m*sizeof(int));
        printf("P%d:\t", i);
        for(int j=0;j<m;j++)
        {
            scanf("%d", &max[i][j]);
        }
    }

    printf("Enter the available resource instances: \n");
    int avail[m];
    for(int i=0;i<m;i++)
    {
        printf("R%d: ", i);
        scanf("%d", &avail[i]);
    }


    printf("Enter the process number (1-indexed): ");
    int r=-1;
    scanf("%d", &r);
    getchar();
    r -= 1;

    printf("Enter its resource request string: ");
    int toRemove[m];
    for(int i=0;i<m;i++)
    {
        printf("R%d: ", i);
        scanf("%d", &toRemove[i]);
    }

    int **need = (int**)malloc(n*sizeof(int*));
    printf("The need matrix is: \n\t");
    for(int i=0;i<m;i++)
    {
        printf("R%d\t", i);
    }
    printf("\n");
    for(int i=0;i<n;i++)
    {
        need[i] = (int*)malloc(m*sizeof(int));
        printf("P%d:\t", i);
        for(int j=0;j<m;j++)
        {
            need[i][j] = max[i][j] - allocation[i][j];
            printf("%d\t", max[i][j] - allocation[i][j]);
        }
        printf("\n");
    }

    printf("The input state is: ");
    
    int* resSq = (int*)malloc(n*sizeof(int));
    int availCopy[m];
    for(int i=0;i<m;i++)
    {
        availCopy[i] = avail[i];
    }
    bool res = bankersSafety(resSq, need, allocation, availCopy, n, m);

    if (res==true)
    {
        printf("Safe state.\n");
        for(int i=0;i<m;i++)
        {
            avail[i] -= toRemove[i];
            if (avail[i]<0)
            {
                printf("Over allocation attempted. Not possible.\n");
                exit(2);
            }
            allocation[r][i] += toRemove[i];
        }
        printf("If request were to be fulfilled,\n");
        printf("The new need matrix: \n\t");
        for(int i=0;i<m;i++)
        {
            printf("R%d\t", i);
        }
        printf("\n");
        for(int i=0;i<n;i++)
        {
            need[i] = (int*)malloc(m*sizeof(int));
            printf("P%d:\t", i);
            for(int j=0;j<m;j++)
            {
                need[i][j] = max[i][j] - allocation[i][j];
                printf("%d\t", max[i][j] - allocation[i][j]);
            }
            printf("\n");
        }
        bool res = bankersSafety(resSq, need, allocation, avail, n, m);
        if (res==true)
        {
            printf("The request can be fulfilled without deadlock.\n");
        }
        else{
            printf("The request cannot be fulfilled.\n");
        }

    }
    else {
        printf("Unsafe state.\n");
    }

}