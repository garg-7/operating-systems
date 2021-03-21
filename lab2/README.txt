Assignment 2
----------------------------------------------------------------------
Question D)

To run:

    gcc -o convert convert.c
    gcc -o count count.c
    gcc d.c
    ./a.out input.txt output.txt

Input file is set as stdin of convert.
Output file is set as stdout of count.
'convert' converts the case and writes to stdout (a pipe).
'count' reads from stdin (the pipe) and writes to stdout.
'count' also displays the number of non-alphabetic characters to stderr.

----------------------------------------------------------------------
Question E)

To run:

    gcc -o player player.c
    gcc e.c
    ./a.out

Two player processes are created within e.c's executable a.out.
The rest of the output is self-explanatory.
Note that at the end when a winner is displayed, 
a win or lose user-defined signal is sent to both the playing
processes. Accordingly, they print to their stderr what their
standing is, therefore 'You have lost' and 'You have won' is displayed
once the winner is declared. To prevent such a display, simply comment
out these lines from the signal handlers in player.c.

Also, the SIGCHLD signal is handled in the parent to prevent zombie
processes.

-----------------------------------------------------------------------
Both the programs are working as expected with no bugs detected.