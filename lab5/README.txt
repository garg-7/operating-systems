-----------------------------------------------------------
ASSIGNMENT 5
CS330
B18CSE011
-----------------------------------------------------------

To execute, run:
gcc Ass5_B18CSE011.c -lpthread
./a.out -b 3 -c 2 -w 1

where b = barber chairs
c = waiting room chairs/sofa capacity
w = waiting room standing capacity

The number of customers during the day are set in a MACRO 
called CUSTOMERS at the top of the code.
It can be changed to whatever value required. (line 9)

The maximum number of customers in the shop i.e. the number
of tokens = b+c+w

The code is completely deadlock free, because the CUSTOMERS
are kept in a single queue and they go from one station to the
next i.e. waiting room standing -> waiting room sofa ->
barber chair -> out. So, a customer at a later stage would
never need a resource of any previous step hence no chance of 
a deadlock.

To prevent starvation, the customers are allotted resources 
after checking where the person who came before you has gotten 
the resources you are seeking or not. If they have, continue, 
otherwise, wait until the person before you has got the resources
you want. This has been implemented with the help of the `stages'
array.

Notes:  The output log is shown for 7 CUSTOMERS. 
        Haircut is given a random period of time between 1 & 5 seconds.
        Cleaning is also given a randomg period of time between 1-5 seconds.
        Customers arrive at random intervals too. 
        Payment at the cash register is accepted in 1-3 seconds randomly.
