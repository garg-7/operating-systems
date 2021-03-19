#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

int main()
{
    char c = ' ';
    while (1)
    {
        c = getchar();
        if (c == EOF)
        {
            exit(0);
        }
        if (islower(c))
        {
            c = toupper(c);
        }
        else if (isupper(c))
        {
            c = tolower(c);
        }
        putchar(c);
    }
    return 0;
}