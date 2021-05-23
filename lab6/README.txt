--------------------------------------------------------
ASSIGNMENT 6
CS330
B18CSE011
--------------------------------------------------------
EXECUTION:

g++ memory.cpp
./a.out 1
Here, 1 is for the number of iterations to be run for
question 1 (page replacement). Enter any other integer
as per the need.

gcc bankers.c
./a.out

--------------------------------------------------------
NOTES:

1. memory.cpp - For this program, for every iteration, 
   the user will be asked to enter the input string of
   reference string which is space separated integers.
   E.g. 1 4 5 2 4 2 2 1 1 0 2 1
   
   It will then run all three page replacement algorithms
   to return the page fault number for each of them.

   Then it will test for Belady's anomaly for the FIFO
   case. This test is conducted by incrementing frame 
   number by 10.

2. bankers.c - For this, the user is asked the number of
   processes and then the number of resource types.
   Then the corresponding allocation, max, available 
   matrices are present in the file t2.txt. Simply copy
   and paste these matrices for input. 
   
   Then input the index of the process number that is 
   asking for additional resources. Followed by the 
   number of resources for all types needed. 

--------------------------------------------------------
   

