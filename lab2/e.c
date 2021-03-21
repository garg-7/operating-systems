#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#define __USE_GNU
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <wait.h>

int main()
{
    srand(time(NULL));
    int n = 7;
    char **colors = (char **)malloc(7 * sizeof(char *));

    char color0[] = "Violet";
    char color1[] = "Indigo";
    char color2[] = "Blue";
    char color3[] = "Green";
    char color4[] = "Yellow";
    char color5[] = "Orange";
    char color6[] = "Red";

    colors[0] = color0;
    colors[1] = color1;
    colors[2] = color2;
    colors[3] = color3;
    colors[4] = color4;
    colors[5] = color5;
    colors[6] = color6;

    // use the first pipe to read from child 1
    int p11[2];
    // another pipe to send confirmation to child 1
    int p12[2];

    int p_check = pipe(p11);
    if (p_check == -1)
    {
        perror("pipe");
        exit(1);
    }

    p_check = pipe(p12);
    if (p_check == -1)
    {
        perror("pipe");
        exit(1);
    }

    // use the second pipe to read from child 2
    int p21[2];
    // another pipe to send confirmation to child 2
    int p22[2];

    p_check = pipe(p21);
    if (p_check == -1)
    {
        perror("pipe2");
        exit(1);
    }

    p_check = pipe(p22);
    if (p_check == -1)
    {
        perror("pipe2");
        exit(1);
    }

    signal(SIGCHLD, SIG_IGN);

    pid_t ch1 = fork();
    if (ch1 == -1)
    {
        perror("fork");
        exit(1);
    }

    else if (ch1 == 0)
    {
        // what the first child needs to do...
        // stdin for this child needs to be the read end
        // of the second pipe created for it.
        // stdout for this child needs to be the write end
        // of the first pipe created for it.

        close(p12[1]); // close write end for the parent to write
        close(p11[0]); // close the read end because it will write to this pipe.

        // close pipes related to second child
        close(p21[1]);
        close(p22[1]);
        close(p21[0]);
        close(p22[0]);

        // write end of pipe to act for stdout of this process
        int d_check = dup2(p11[1], STDOUT_FILENO);
        if (d_check == -1)
        {
            perror("dup1 child 1");
            exit(1);
        }

        // read end of pipe 2 to act for stdin of this process
        d_check = dup2(p12[0], STDIN_FILENO);
        if (d_check == -1)
        {
            perror("dup2 child 1");
            exit(1);
        }
        sleep(3);
        int r_check = execl("player", "player", NULL);
        if (r_check == -1)
        {
            perror("player 1");
            exit(1);
        }
    }

    else
    {
        // printf("%d\n", ch1);
        pid_t ch2 = fork();
        if (ch2 == -1)
        {
            perror("fork2");
            exit(1);
        }

        else if (ch2 == 0)
        {
            // what the second child needs to do...
            // srand(time(NULL));
            close(p22[1]); //close write end of pipe from which it reads
            close(p21[0]); // close read end as it will write to it.

            // close the other child's pipes' descriptors
            close(p11[1]);
            close(p12[1]);
            close(p11[0]);
            close(p12[0]);

            // write end of pipe to act for stdout of this process
            int d_check = dup2(p21[1], STDOUT_FILENO);
            if (d_check == -1)
            {
                perror("dup1 child 2");
                exit(1);
            }

            // read end of pipe 2 to act for stdin of this process
            d_check = dup2(p22[0], STDIN_FILENO);
            if (d_check == -1)
            {
                perror("dup2 child 2");
                exit(1);
            }
            sleep(1);
            int r_check = execl("player", "player", NULL);
            if (r_check == -1)
            {
                perror("player 2");
                exit(1);
            }
        }

        else
        {
            // printf("%d\n", ch2);
            // what the parent does...
            // printf("Parent.\n");
            //close write ends of the pipes from which it reads.
            close(p11[1]);
            close(p21[1]);

            // close read ends of the pipes to which it will write
            close(p12[0]);
            close(p22[0]);

            int rfd1 = p11[0]; // parent reads from child 1
            int rfd2 = p21[0]; // parent reads from child 2

            int wfd1 = p12[1];
            int wfd2 = p22[1];

            int ch1_score = 0, ch2_score = 0;

            // parent's choice
            int choice = ' ';

            // first child's guess
            char guess1[5];

            // second child's guess
            char guess2[5];

            char response;

            while (1)
            {
                // loop runs till one child wins the game.
                // srand(time(NULL));
                // create parent's choice
                choice = (char)((random() % 7) + '0');
                printf("Parent's choice: %s.\n", colors[choice - '0']);

                // int check_d = fcntl(rfd1, F_GETFD);
                // printf("%d\n", check_d);
                // get children's guesses
                int read_check = read(rfd1, guess1, sizeof(guess1));
                if (read_check == -1)
                {
                    perror("read from child 1's pipe");
                    // fprintf(stderr, "%d", errno);
                    exit(1);
                }
                // printf("I have received the first child's guess.\n");
                read_check = read(rfd2, guess2, sizeof(guess2));
                // printf("Read check value: %d.\n", read_check);
                if (read_check == -1)
                {
                    perror("read from child 2's pipe");
                    exit(1);
                }
                // printf("%c, %c\n",(guess1[0]),(guess2[0]));
                printf("Player 1 guessed: %s and scored: %d points.\n", colors[guess1[0] - '0'], abs(choice - guess1[0]));
                printf("Player 2 guessed: %s and scored: %d points.\n", colors[guess2[0] - '0'], abs(choice - guess2[0]));

                // calculate scores of the children after this round.
                ch1_score += abs(choice - guess1[0]);
                ch2_score += abs(choice - guess2[0]);

                if (guess1[0] == choice && guess2[0] == choice)
                {
                    //both guessed correctly, ignore the round.
                    printf("Current Scores-\nP1: %d, P2: %d\n", ch1_score, ch2_score);
                    printf("Both guessed correctly. Moving onto the next round...\n\n");
                    // response = 'c';
                    // write(wfd1, &response, sizeof(char));
                    // write(wfd2, &response, sizeof(char));
                }

                else
                {
                    if (guess1[0] == choice)
                    {
                        int k_check = kill(ch1, SIGUSR1); // child 1 won
                        // printf("[%d]", k_check);
                        if (k_check == -1)
                        {
                            perror("killing child 1");
                            exit(1);
                        }

                        k_check = kill(ch2, SIGUSR2); // child 2 lost
                        // printf("[%d]", k_check);
                        if (k_check == -1)
                        {
                            perror("killing child 2");
                            exit(1);
                        }

                        printf("Player 1 has won.\n");
                        exit(0);
                    }

                    else if (guess2[0] == choice)
                    {
                        int k_check = kill(ch1, SIGUSR2); // child 1 lost
                        if (k_check == -1)
                        {
                            perror("killing child 1");
                            exit(1);
                        }

                        k_check = kill(ch2, SIGUSR1); // child 2 won
                        if (k_check == -1)
                        {
                            perror("killing child 2");
                            exit(1);
                        }

                        printf("Player 2 has won.\n");
                        exit(0);
                    }

                    else if (ch1_score >= 50)
                    {
                        int k_check = kill(ch1, SIGUSR1); // child 1 won
                        if (k_check == -1)
                        {
                            perror("killing child 1");
                            exit(1);
                        }

                        if (ch2_score >= 50)
                        {
                            k_check = kill(ch2, SIGUSR1); // child 2 won too
                            if (k_check == -1)
                            {
                                perror("killing child 2");
                                exit(1);
                            }
                            printf("Both the players have won.\n");
                            printf("Current Scores-\nP1: %d, P2: %d\n\n", ch1_score, ch2_score);
                        }
                        else
                        {
                            k_check = kill(ch2, SIGUSR2); // child 2 lost
                            if (k_check == -1)
                            {
                                perror("killing child 2");
                                exit(1);
                            }
                            printf("Player 1 has won.\n");
                            printf("Current Scores-\nP1: %d, P2: %d\n\n", ch1_score, ch2_score);
                        }
                        exit(0);
                    }

                    else if (ch2_score >= 50)
                    {
                        int k_check = kill(ch2, SIGUSR1); // child 2 won
                        if (k_check == -1)
                        {
                            perror("killing child 2");
                            exit(1);
                        }
                        if (ch1_score >= 50)
                        {
                            k_check = kill(ch1, SIGUSR1); // child 1 won too
                            if (k_check == -1)
                            {
                                perror("killing child 1");
                                exit(1);
                            }
                            printf("Both the players have won.\n");
                            printf("Current Scores-\nP1: %d, P2: %d\n\n", ch1_score, ch2_score);
                        }
                        else
                        {
                            k_check = kill(ch1, SIGUSR2); // child 1 lost
                            if (k_check == -1)
                            {
                                perror("killing child 1");
                                exit(1);
                            }
                            printf("Player 2 has won.\n");
                            printf("Current Scores-\nP1: %d, P2: %d\n\n", ch1_score, ch2_score);
                        }
                        exit(0);
                    }

                    else
                    {
                        // no one has won.
                        // just ask the players to put the next guess in stdout.
                        // response = 'c';
                        // write(wfd1, &response, sizeof(char));
                        // write(wfd2, &response, sizeof(char));
                        printf("Current Scores-\nP1: %d, P2: %d\n", ch1_score, ch2_score);
                        printf("No one has won yet. Moving to the next round...\n\n");
                    }
                }
            }
        }
    }
}