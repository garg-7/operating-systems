#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main()
{
    char c = ' ';
    int count = 0;
    while (1)
    {
        c = getchar();
        if (c == EOF)
        {
            fprintf(stderr, "Count of non-alphabetic characters: %d.\n", count);
            exit(0);
        }
        if (!(isalpha(c)))
        {
            count += 1;
        }
        putchar(c);
    }
    return 0;
}