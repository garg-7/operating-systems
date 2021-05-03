---------------------------------------------------------
Lab Assignment 7
CS330

B18CSE011
---------------------------------------------------------

EXECUTION:

g++ paging.cpp
./a.out

---------------------------------------------------------

Note: One after the other, the four values i.e.
k, m, f, s are asked to be input. 

Degree of multiprogramming has been assumed to be 1
to avoid complications.

Note that to be able to make full use of the TLB
LRU replacement algorithm has been used to replace
the (page,frame) pair into the TLB at every miss
after bringing it into the main memory if so required.
Note that this means that LRU is implemented twice.

Once between the TLB and the main memory.
Then again between the main memory and the backing store.

There may be some time delays in between, to re-seed
the random number generator using current time for 
better randomness.

The required output is shown.

Tested to be working on Ubuntu 20.04.
---------------------------------------------------------