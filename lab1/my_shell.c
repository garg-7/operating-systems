#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

#define Q_STARTED 1
#define Q_ENDED 0
#define PENDING 1
#define OVER 0
#define NO 0
#define YES 1
#define READ_END 0
#define WRITE_END 1

int listDir(char *dirToList)
{
    DIR *d;
    struct dirent *dirFlow;
    d = opendir(dirToList);
    int isValidDir = 0;
    if (d != NULL)
    {
        isValidDir = 1;
        printf("%s:\n", dirToList);
        while ((dirFlow = readdir(d)) != NULL)
        {
            printf(" %s\n", dirFlow->d_name);
        }
        printf("\n");
        closedir(d);
    }
    // if (isValidDir==0)
    //     printf("%s: No such file or directory\n", dirToList);
    return isValidDir;
}

/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    int s_quote = Q_ENDED;
    int d_quote = Q_ENDED;
    int pendingWord = OVER;
    for (i = 0; i < strlen(line); i++)
    {

        char readChar = line[i];
        // printf("%c: ", readChar);
        if (readChar == ' ' || readChar == '\t')
        {
            // printf("At a whitespace\n");
            if (s_quote == Q_ENDED && d_quote == Q_ENDED)
            {
                // printf("Quotes are ended\n");
                if (pendingWord == PENDING)
                {
                    // printf("Finish pending word\n");
                    token[tokenIndex] = '\0';
                    if (tokenIndex != 0)
                    {
                        tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                        strcpy(tokens[tokenNo++], token);
                        tokenIndex = 0;
                        pendingWord = OVER;
                    }
                }
            }
            else
            {
                // printf("Space within quotes\n");
                token[tokenIndex++] = readChar;
                pendingWord = PENDING;
            }
        }
        else if (readChar == '\n')
        {
            // if new line is reached, catch the case where
            // a quote hasn't been closed properly.
            if (pendingWord == PENDING)
            {
                token[tokenIndex] = '\0';
                if (tokenIndex != 0)
                {
                    tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                    strcpy(tokens[tokenNo++], token);
                    tokenIndex = 0;
                    pendingWord = OVER;
                }
            }
        }
        else if (readChar == '\'')
        {
            token[tokenIndex++] = readChar;
            if (s_quote == Q_STARTED)
            {
                // printf("End s-quote add token to tokens\n");
                token[tokenIndex] = '\0';
                if (tokenIndex != 0)
                {
                    tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                    strcpy(tokens[tokenNo++], token);
                    tokenIndex = 0;
                }
                pendingWord = OVER;
                s_quote = Q_ENDED;
            }
            else
            {
                // printf("Start s-quote\n");
                s_quote = Q_STARTED;
                pendingWord = PENDING;
            }
        }
        else if (readChar == '\"')
        {
            token[tokenIndex++] = readChar;
            if (d_quote == Q_STARTED)
            {
                // printf("End d-quote add token to tokens\n");
                token[tokenIndex] = '\0';
                if (tokenIndex != 0)
                {
                    tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                    strcpy(tokens[tokenNo++], token);
                    tokenIndex = 0;
                }
                pendingWord = OVER;
                d_quote = Q_ENDED;
            }
            else
            {
                // printf("Start d-quote\n");
                d_quote = Q_STARTED;
                pendingWord = PENDING;
            }
        }
        else
        {
            // printf("add simple letter\n");
            token[tokenIndex++] = readChar;
            pendingWord = PENDING;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}

void clean_token(char *source, char *dest)
{
    if (source[0] != 39 && source[0] != 34)
    {
        strcpy(dest, source);
    }
    else
    {
        int dest_filler = 0;
        for (int i = 1; i < strlen(source) - 1; i++)
        {
            dest[dest_filler] = source[i];
            dest_filler += 1;
        }
    }
}

void changeDir(char *newPath)
{
    char *previousDir = (char *)malloc(PATH_MAX);
    getcwd(previousDir, PATH_MAX);
    char *processed_token = (char *)malloc(sizeof(char) * MAX_TOKEN_SIZE);
    clean_token(newPath, processed_token);
    chdir(processed_token);
    char *currentDir = (char *)malloc(PATH_MAX);
    getcwd(currentDir, PATH_MAX);
    if (strcmp(previousDir, currentDir) == 0)
    {
        printf("Working directory unchanged.\n");
    }
    else
    {
        printf("Working directory changed to: %s\n", currentDir);
    }
    free(processed_token);
    free(previousDir);
    free(currentDir);
    return;
}

void childZombieHandler(int status)
{
    wait(NULL);
}

int main(int argc, char *argv[])
{
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i;

    struct utsname unameData;
    uname(&unameData);

    FILE *fp;
    if (argc == 2)
    {
        fp = fopen(argv[1], "r");
        if (fp < 0)
        {
            printf("File doesn't exist.");
            return -1;
        }
    }

    while (1)
    {
        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        if (argc == 2)
        { // batch mode
            if (fgets(line, sizeof(line), fp) == NULL)
            { // file reading finished
                break;
            }
            line[strlen(line)] = '\0';
        }
        else
        { // interactive mode
            char *currentDir = (char *)malloc(PATH_MAX);
            getcwd(currentDir, PATH_MAX);
            printf("%s@%s:%s$ ", getenv("USERNAME"), unameData.nodename, currentDir);
            free(currentDir);
            scanf("%[^\n]", line);
            getchar();
        }

        // printf("Command entered: %s (remove this debug output later)\n", line);
        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);

        int num_tokens = 0;
        int isPipePresent = NO;
        int *pipeLocs = (int *)malloc(MAX_NUM_TOKENS * sizeof(int));
        for (int i = 0; i < MAX_NUM_TOKENS; i++)
        {
            if (tokens[i] == NULL)
            {
                num_tokens = i;
                break;
            }
            else
            {
                if (strcmp(tokens[i], "|") == 0)
                {
                    pipeLocs[isPipePresent++] = i;
                    // printf("|%d|", i);
                }
            }
        }

        // printf("Number of tokens: %d\n", num_tokens);
        if (isPipePresent == NO)
        {
            if (strcmp(tokens[0], "exit") == 0)
            {
                printf("Exiting shell...\n");
                exit(0);
            }

            else if (strcmp(tokens[0], "ls") == 0)
            {
                // doing the general thing first.
                if (tokens[1] == NULL)
                {
                    listDir(".");
                }
                else
                {
                    if (strcmp(tokens[num_tokens - 1], "&") != 0)
                    {
                        for (int i = 1; i < num_tokens; i++)
                        {
                            char *processed_token = (char *)malloc(sizeof(char) * MAX_TOKEN_SIZE);
                            clean_token(tokens[i], processed_token);
                            int isValidDir = 0;
                            isValidDir = listDir(processed_token);
                            if (isValidDir == 0)
                            {
                                // if the name wasn't a valid dir check if it's a file
                                int isFile = 1;
                                isFile = access(processed_token, F_OK);
                                // printf("file check returned: %d\n", isFile);
                                if (isFile == 0)
                                {
                                    // printf("It is a file");
                                    printf("%s\n", processed_token);
                                }
                                else
                                {
                                    // not even a valid file
                                    printf("%s: No such file or directory\n", processed_token);
                                }
                            }
                            else
                            {
                                // path was a directory do nothing
                            }
                            free(processed_token);
                        }
                    }
                    else
                    {
                        pid_t childPID = fork();
                        if (childPID == -1)
                        {
                            printf("fork failed to create a child process.\n");
                        }
                        else
                        {
                            if (childPID == 0)
                            {
                                // what the child process should do

                                int checkForFailure = 0;
                                char **newArgs = (char **)malloc(num_tokens * sizeof(int *));
                                for (int i = 0; i < num_tokens; i++)
                                {
                                    if (strcmp(tokens[i], "&") == 0)
                                    {
                                        newArgs[i] = NULL;
                                        break;
                                    }
                                    else
                                    {
                                        newArgs[i] = tokens[i];
                                    }
                                }
                                checkForFailure = execvp(newArgs[0], newArgs);
                                if (checkForFailure == -1)
                                {
                                    printf("Execution of the requested command failed.\n");
                                    if (errno == ENOENT)
                                    {
                                        printf("Command {%s} not found\n", tokens[0]);
                                    }
                                    else if (errno == EACCES)
                                    {
                                        printf("Unable to execute the command {%s}. Could be an issue with the permissions.\n", tokens[0]);
                                    }
                                }
                            }
                            else
                            {
                                signal(SIGCHLD, childZombieHandler);
                            }
                        }
                    }
                }
            }

            else if (strcmp(tokens[0], "pwd") == 0)
            {
                if (strcmp(tokens[0], "&") != 0)
                {
                    printf("Current working directory:\n");
                    char *currentPathResult = (char *)malloc(PATH_MAX);
                    printf("%s\n", getcwd(currentPathResult, PATH_MAX));
                    free(currentPathResult);
                }

                else
                {
                    pid_t childPID = fork();
                    if (childPID == -1)
                    {
                        printf("fork failed to create a child process.\n");
                    }
                    else
                    {
                        if (childPID == 0)
                        {
                            // what the child process should do

                            int checkForFailure = 0;
                            char **newArgs = (char **)malloc(num_tokens * sizeof(int *));
                            for (int i = 0; i < num_tokens; i++)
                            {
                                if (strcmp(tokens[i], "&") == 0)
                                {
                                    newArgs[i] = NULL;
                                    break;
                                }
                                else
                                {
                                    newArgs[i] = tokens[i];
                                }
                            }
                            checkForFailure = execvp(newArgs[0], newArgs);
                            if (checkForFailure == -1)
                            {
                                printf("Execution of the requested command failed.\n");
                                if (errno == ENOENT)
                                {
                                    printf("Command {%s} not found\n", tokens[0]);
                                }
                                else if (errno == EACCES)
                                {
                                    printf("Unable to execute the command {%s}. Could be an issue with the permissions.\n", tokens[0]);
                                }
                            }
                        }
                        else
                        {
                            signal(SIGCHLD, childZombieHandler);
                        }
                    }
                }
            }

            else if (strcmp(tokens[0], "cd") == 0)
            {

                // entering only cd changes the working directory to the home dir.
                // so handling that first
                if (tokens[1] == NULL)
                {
                    chdir(getenv("HOME"));
                }
                else
                {
                    if (num_tokens > 2)
                    {
                        printf("cd takes just 1 argument, got %d\n", num_tokens - 1);
                        printf("Using the first argument {%s}\n", tokens[1]);
                        changeDir(tokens[1]);
                    }
                    else
                    {
                        changeDir(tokens[1]);
                    }
                }
            }

            else
            {
                pid_t childPID = fork();
                if (childPID == -1)
                {
                    printf("fork failed to create a child process.\n");
                }
                else
                {
                    if (childPID == 0)
                    {
                        // what the child process should do

                        int checkForFailure = 0;
                        char **newArgs = (char **)malloc(num_tokens * sizeof(int *));
                        for (int i = 0; i < num_tokens; i++)
                        {
                            if (strcmp(tokens[i], "&") == 0)
                            {
                                newArgs[i] = NULL;
                                break;
                            }
                            else
                            {
                                newArgs[i] = tokens[i];
                            }
                        }
                        checkForFailure = execvp(newArgs[0], newArgs);
                        if (checkForFailure == -1)
                        {
                            printf("Execution of the requested command failed.\n");
                            if (errno == ENOENT)
                            {
                                printf("Command {%s} not found\n", tokens[0]);
                            }
                            else if (errno == EACCES)
                            {
                                printf("Unable to execute the command {%s}. Could be an issue with the permissions.\n", tokens[0]);
                            }
                        }
                    }
                    else
                    {
                        // what the parent process should do
                        // parent should wait if it's run in foreground
                        // else start waiting for next command.
                        if (strcmp(tokens[num_tokens - 1], "&") == 0)
                        {
                            //run in background so continue
                            //there is the issue of zombie child processes
                            //so handle that by ignoring the SIGCHLD signal
                            // or creating a separate handler that waits for
                            // the SIGCHLD signal
                            // printf("Child will run in the background. \n");
                            signal(SIGCHLD, childZombieHandler);
                        }
                        else
                        {
                            // run in foreground, so wait for the child.
                            // printf("Have to wait for the child.\n");
                            int status = waitpid(childPID, NULL, 0);
                            if (status == childPID)
                            {
                                // printf("Child exited normally.\n");
                            }
                            else
                            {
                                printf("The process did not exit normally.\n");
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // piping has been used.
            // create as many pipes as are needed with the help
            // of the isPipePresent count.
            // approach: iterate over the tokens and accumulate tokens
            // for a (supposed) command.
            // When a pipe is reached. Fork a child process, set up output descriptor
            // to the write of the pipe and execute the child. In the meantime, the
            // parent continues iterating. Again, when a pipe is reached. Fork a child
            // process, set up the stdin to read from the previous pipe's read end.
            // if there is a pipe left, then set up the output descriptor to the write
            // end of the next pipe. Maintain a count of settled pipes.
            int **pipes = (int **)malloc(sizeof(int *) * isPipePresent);
            for (int i = 0; i < isPipePresent; i++)
            {
                int *currentPipe = (int *)malloc(sizeof(int) * 2);
                pipes[i] = currentPipe;
                int isSuccess = pipe(currentPipe); // [0] = read end, [1] = write end
                if (isSuccess == -1)
                {
                    printf("Failed to create the necessary pipes.\n");
                    exit(1);
                }
                printf("pipe created.\n");
            }

            // pipes have been created.

            char ***tokenSections = (char ***)malloc((isPipePresent + 1) * sizeof(char **));
            if (tokenSections == NULL)
            {
                printf("Malloc failed1.\n");
            }
            // create the individual token sections
            int currentPipeIndex = 0;
            char **currentTokenSection = (char **)malloc(num_tokens * sizeof(char *));
            if (currentTokenSection == NULL)
            {
                printf("Malloc failed2.\n");
            }
            int tokenWithinSection = 0;
            for (int i = 0; i < num_tokens + 1; i++)
            {
                // printf("token: %s ", tokens[i]);
                if (i == pipeLocs[currentPipeIndex] || i == num_tokens)
                {
                    // printf("current added to main.\n");
                    currentTokenSection[tokenWithinSection] = NULL;
                    tokenSections[currentPipeIndex++] = currentTokenSection;
                    currentTokenSection = (char **)malloc(num_tokens * sizeof(char *));
                    tokenWithinSection = 0;
                }
                else
                {
                    // printf("added to current\n");
                    currentTokenSection[tokenWithinSection++] = tokens[i];
                }
            }
            // for (int i=0;i<(isPipePresent+1);i++) {
            //     printf("[");
            //     for (int j=0;j<num_tokens; j++) {
            //         if (tokenSections[i][j]!=NULL)
            //         printf("%s :: ", tokenSections[i][j]);
            //     }
            //     printf("]\n");
            // }
            // exit(0);
            int *pipesHandled = (int *)malloc(sizeof(int));
            *pipesHandled = 0;
            for (int i = 0; i < (isPipePresent + 1); i++)
            {
                // pipe is reached. so fork().
                pid_t childPID = fork();
                if (childPID == -1)
                {
                    printf("fork failed to create a child process.\n");
                }
                else
                {
                    if (childPID == 0)
                    {
                        // for the child...
                        // modify descriptor(s) and exec

                        // if it is in the middle then change stdin
                        printf("child is setting up for the pipeline.\n");
                        for (int i = 0; i < num_tokens; i++)
                        {
                            if (tokenSections[*pipesHandled][i] != NULL)
                                printf("{%s}", tokenSections[*pipesHandled][i]);
                        }
                        if (*pipesHandled > 0)
                        {
                            printf("setting stdin to pipe %d.\n", *pipesHandled - 1);
                            int checkDup = 0;
                            checkDup = dup2(pipes[*pipesHandled - 1][READ_END], STDIN_FILENO);
                            if (checkDup == -1)
                            {
                                printf("Dup failed to set stdin to read end of pipe for command: %s\n", tokenSections[*pipesHandled][0]);
                                printf("Pipeline interrupted.\n");
                                exit(1);
                            }
                            close(pipes[*pipesHandled - 1][WRITE_END]);
                            close(pipes[*pipesHandled - 1][READ_END]);
                        }

                        // if it isn't the last pipe, change stdout
                        if (*pipesHandled < isPipePresent)
                        {
                            printf("setting stdout to pipe %d.\n", *pipesHandled);
                            int checkDup = 0;
                            checkDup = dup2(pipes[*pipesHandled][WRITE_END], STDOUT_FILENO);
                            if (checkDup == -1)
                            {
                                printf("Dup failed to set stdout to write end of pipe for command: %s\n", tokenSections[*pipesHandled][0]);
                                printf("Pipeline interrupted.\n");
                                exit(1);
                            }
                            close(pipes[*pipesHandled][WRITE_END]);
                            close(pipes[*pipesHandled][READ_END]);
                        }

                        int checkExec = 0;
                        // printf("running exec.\n");
                        checkExec = execvp(tokenSections[*pipesHandled][0], tokenSections[*pipesHandled]);
                        if (checkExec == -1)
                        {
                            printf("Execution of the requested command failed.\n");
                            if (errno == ENOENT)
                            {
                                printf("Command {%s} not found\n", tokenSections[*pipesHandled][0]);
                            }
                            else if (errno == EACCES)
                            {
                                printf("Unable to execute the command {%s}. Could be an issue with the permissions.\n", tokenSections[*pipesHandled][0]);
                            }
                        }
                    }
                    else
                    {
                        waitpid(childPID, NULL, 0);
                        // wait(NULL);
                        // parent should wait for the first process to do it's thing
                        // otherwise, the indexing in tokenSections may not work out.
                        *pipesHandled += 1;
                        printf("I was here\n");
                    }
                }
            }
            printf("Pipeline execution completed.\n");
        }

        // for(i=0;tokens[i]!=NULL;i++){
        //     printf("found token %s (remove this debug output later)\n", tokens[i]);
        // }

        // Freeing the allocated memory
        for (i = 0; tokens[i] != NULL; i++)
        {
            free(tokens[i]);
        }
        free(tokens);
    }
    return 0;
}
