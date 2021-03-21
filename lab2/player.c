// Child process (or Player) for the "mind game".

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void winhandler(int sig)
{
    // you have won
    fprintf(stderr, "You have won.\n");
    // pid_t ppid = getppid();
    // kill(ppid, SIGCHLD);
    exit(0);
}

void lossHandler(int sig)
{
    // you have lost
    fprintf(stderr, "You have lost.\n");
    // pid_t ppid = getppid();
    // kill(ppid, SIGCHLD);
    exit(0);
}

int main()
{
    srand(time(NULL));
    int n = 7;
    char choice = ' ';
    signal(SIGUSR1, winhandler);  // handle the SIGUSR1 signal that you will get if you win
    signal(SIGUSR2, lossHandler); // handle the SIGUSR2 signla that you get if you lose

    while (1)
    {
        // srand(time(NULL));
        choice = (char)((random() % 7) + '0');
        putc(choice, stdout);
    }
}