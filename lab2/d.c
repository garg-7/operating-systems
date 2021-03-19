#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Please pass input and output file names.\n");
        exit(1);
    }
    if (argc > 3)
    {
        fprintf(stdout, "More than 2 arguments passed. Considering the first 2...\n");
    }

    int ip = open(argv[1], O_RDONLY);
    if (ip == -1)
    {
        fprintf(stderr, "Unable to open input file to read.\n");
        exit(1);
    }

    int op = creat(argv[2], 0644);
    if (op == -1)
    {
        fprintf(stderr, "Unable to open output file to write.\n");
        exit(1);
    }

    int d_check = dup2(ip, STDIN_FILENO);
    if (d_check == -1)
    {
        fprintf(stderr, "Unable to duplicate input file descriptor onto stdin.\n");
        exit(1);
    }
    close(ip);

    d_check = dup2(op, STDOUT_FILENO);
    if (d_check == -1)
    {
        fprintf(stderr, "Unable to duplicate output file descriptor onto stdout.\n");
        exit(1);
    }
    close(op);

    int pipeDes[2];
    int p = pipe(pipeDes);
    if (p == -1)
    {
        fprintf(stderr, "Unable to create pipe.\n");
        exit(1);
    }

    pid_t ch1 = fork();
    if (ch1 == -1)
    {
        fprintf(stderr, "Unable to fork a child process.\n");
        exit(1);
    }

    if (ch1 == 0)
    {
        // first set up the child that reads from the pipe.

        close(pipeDes[1]);
        d_check = dup2(pipeDes[0], STDIN_FILENO);
        if (d_check == -1)
        {
            fprintf(stderr, "Child unable to duplicate the pipe's read end to stdin.\n");
            exit(1);
        }

        int ret_check = execl("count", "count", (char *)0);
        if (ret_check == -1)
        {
            fprintf(stderr, "There was some problem while executing count.\n");
            exit(1);
        }
    }

    else
    {
        pid_t ch2 = fork();
        if (ch2 == -1)
        {
            fprintf(stderr, "Unable to fork a second child process.\n");
            exit(1);
        }

        if (ch2 == 0)
        {
            // the child that writes to the pipe. 
            // technically, this will write first, and then the other
            // will read from the pipe to do further operations.
            // but this needs to be after the reader because a process 
            // can write to a pipe only if someone else is ready to 
            // read from it.

            close(pipeDes[0]);
            d_check = dup2(pipeDes[1], STDOUT_FILENO);
            if (d_check == -1)
            {
                fprintf(stderr, "Child unable to duplicate the pipe's write end to stdout.\n");
                exit(1);
            }

            int ret_check = execl("convert", "convert", (char *)0);
            if (ret_check == -1)
            {
                fprintf(stderr, "There was some problem while executing convert.\n");
            }
        }

        else
        {
            close(pipeDes[0]);
            close(pipeDes[1]);

            wait(NULL);
            wait(NULL);

            return 0;
        }
    }
}