--------------------------------------------------------------
Lab Assignment - 3
CS330
Submitted by - B18CSE011
--------------------------------------------------------------

The files - 
1. generator.c -> used to generate the process arrival
table.

2. simulate.c -> used to simulate the various scheduling
algorithms

3. driver.py -> used to drive the experiments mentioned
in the Q.c), using the above 2 files.

--------------------------------------------------------------

How to execute:

To run the experiments as mentioned in the assignment's
Q.c) simply run - 

python driver.py

(Note that the dependency of matplotlib needs to be installed
in order to be able to run the above file.)

This execution will create a Results folder in the current
directory. This folder will contain the 9 plots required.
It will also contain the outputs of the scheduling algorithms
in the respective folders.

E.g. Results/20_processes/Trial_5/ represents the folder
created for N = 20 processes and 5th trial (out of 10).
In every such folder, following files would be present:
    - data.txt -> contains the process arrival trace
    - FCFS.txt -> First come first serve
    - NSJF.txt -> Non-preemptive shortest job first
    - PSJF.txt -> Preemptive shortest job first
    - P.txt -> Priority based scheduling
    - RR.txt -> Round-robin scheduling

All these files contain the timeline - as to when a process
obtains access to the CPU, when it gets preempted (if it does),
and when it gets completed acc to the algorithm in the
name of the file.

NOTE: Priority based scheduling has been implemented without
preemption.

--------------------------------------------------------------

To run any singular experiment:

1. First generate data using -

gcc -o gen generator.c -lm
./gen N m
where N and m are values passed via cmd line. 
N (int) = number of processes.
m (float) = mean of the exponential distribution (2 works well)

2. Now execute whatever simulation algorithm is desired - 

gcc -o sim simulate.c
./sim FCFS
In place of FCFS, any desired algorithm can be passed, 
PSJF -> for Preemptive shortest job first
NSJF -> for Non-preemptive shortest job first
RR -> for Round-robin scheduling
P -> for Priority based scheduling

The results would be displayed on stdout.
-------------------------------------------------------------

No bugs have been found, while running any of the above
code files.

-----------------------------END-----------------------------
