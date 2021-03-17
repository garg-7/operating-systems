Assignment 1 - Designing my own shell.

RUN (interactive mode)
my_shell.c
./a.out

RUN (batch mode)
my_shell.c
./a.out script.txt
Internal commands supported:
ls
pwd
cd
exit
NOTE: If run without &, they do not spawn another process, as
asked in the requirements. However, if run with &, they spawn another
process to be able to do the execution in the background.


External commands supported:
execvp() has been used to support all other commands besides 
the above mentioned ones. 
Whether the execution is internal or external a new process is spawned.
'&' can be used in the end to run in the background.

Foreground and background execution:
Commands ending with an ampersand are executed in the background
as expected. 

Exception Handling:
At multiple places within the shell program, wherever system calls 
are made, their returned value is checked to be able to indicate 
the error (if any) to the user.

Mode of operation: 
Batch mode is working in the same way as the interactive one. To run a file 
in batch mode pass a file as an argument: 
./a.out script.txt

Pipe implementation: 
Piping has been implemented for as many levels of pipes as needed.
The pipes are automatically counted and the execution is performed 
accordingly. If there is any problem while the pipeline is being 
executed, the program exits after indicating that the pipeline 
couldn't be executed.
E.g.  the following command works as expected
pwd | ls 

Additional features: 
1. As ls has been implemented manually, it works with a path (if the user decides to pass one.),
not just the current directory.
2. The tokenizer has been modified to ignore tokenizing at whitespaces
if the space is within quotes - This enables the user to work with 
commands like ls "folder name" i.e. folders/files with space in their name.
3. SIGCHLD handler has been used to prevent defunct zombie processes.
4. Improved layout of the shell, by adding username and machine name into the 
shell.

Known bugs:
When cat is used at the end of a pipe, the shell goes in waiting state.
However, this issue is also encountered in the actual terminal where it can be resolved 
by Ctrl+C. 