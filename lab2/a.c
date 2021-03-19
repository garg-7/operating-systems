#include <stdio.h>
#define NO 0
#define YES 1

int main(int argc, char *argv[])
{
    int FOUND = NO;
    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
        {
            printf("%s ", argv[i]);
            FOUND = YES;
        }
    }
    if (FOUND == YES)
        printf("\n");
    return 0;
}