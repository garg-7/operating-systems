#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <random>
#include <unistd.h>
using namespace std;

int main()
{
    srand(time(NULL));
    int k= -1, m = -1, f = -1, s = -1;
    printf("Enter the number of processes (k): ");
    scanf("%d", &k);

    printf("Enter the number of max pages needed per process (m): ");
    scanf("%d", &m);

    printf("Enter the number of frames in main memory (f): ");
    scanf("%d", &f);

    if(f>=m)
    {
        printf("m needs to be greater than f.\n");
        exit(1);
    }

    printf("Enter the size of TLB (s): ");
    scanf("%d", &s);

    if (s>=f)
    {
        printf("s needs to be less than f.\n");
        exit(1);
    }


    vector<int> processes(k);

    // assigning integers to processes
    iota(processes.begin(), processes.end(), 0);
    
    // shuffling processes
    shuffle(processes.begin(), processes.end(), default_random_engine(0));
    
    vector<int>::iterator t; 
    for(t = processes.begin(); t!=processes.end(); ++ t)
    {
        printf("Process [%d] dispatched.\n", *t);
        vector<pair<int, int>> tlb(s);      // pagenum, framenum
        for (int i=0;i<s;i++)
        {
            tlb[i].first = -1;
            tlb[i].second = -1;
        }
        list<int> tlbLRU;


        int faults = 0;
        int m_i = (float)random()/RAND_MAX * (m-f) + f;
        
        vector<pair<int, int>> pt(m_i);     // framenum, validity
        for (int i=0;i<m_i;i++)
        {
            pt[i].first = -1;
            pt[i].second = -1;
        }
        list<int> ptLRU;

        // the f frames in the main memory.
        // what page every frame holds.
        vector<int> main_mem(f);
        for(int i=0;i<f;i++)
        {
            main_mem[i] = -1;
        }

        for(int i=0;i<m_i;i++)
        {
            pt[i].first = -1;
            pt[i].second = -1;
        }

        // printf("m_i: %d\n", m_i);
        
        // generate length of reference string
        sleep(1);
        srand(time(NULL));
        int lenRef = (float)random()/RAND_MAX * 8 * m_i + 2 * m_i;

        // printf("ref str len: %d \n", lenRef);
        char ref[lenRef+1][100];
        
        // generate the reference string
        for (int i=0;i<lenRef;i++)
        {
            sprintf(ref[i], "%d", (int)((float)random()/RAND_MAX * m_i));
        }
        ref[lenRef][0] = 0;

        printf("The reference string: ");
        for(int i=0;i<lenRef;i++)
        {
            printf("%s ", ref[i]);
        }
        printf("\n");
        // traverse the reference string
        for (int i=0;i<lenRef; i++)
        {
            // check in TLB first
            bool hit = false;
            for(int j=0;j<s;j++)
            {
                if (tlb[j].first==atoi(ref[i]))
                {
                    // TLB hit
                    printf("Process %d: for page %d, TLB hit -> with frame %d\n", *t, tlb[j].first, tlb[j].second);
                    tlbLRU.remove( tlb[j].first);
                    tlbLRU.push_front(tlb[j].first);
                    hit = true;
                    break;
                }
            }
            
            if (hit==false)
            {
                // TLB miss
                // check in the page table whether it's in the
                // memory
                if (pt[atoi(ref[i])].second > -1)
                {
                    printf("Process %d: for page reference %d, TLB miss -> Page table valid -> with frame number %d\n", *t, atoi(ref[i]), pt[atoi(ref[i])].first);
                    ptLRU.remove(atoi(ref[i]));
                    ptLRU.push_front(atoi(ref[i]));
                    // page is in the main memory
                    // have to bring it to the TLB

                    // does TLB have some space
                    int hasSpace = -1;
                    for(int k=0;k<s;k++)
                    {
                        if (tlb[k].first == -1)
                        {
                            hasSpace = k;
                            break;
                        }
                    }

                    if (hasSpace>-1)
                    {
                        tlb[hasSpace].first = atoi(ref[i]);
                        tlb[hasSpace].second = pt[atoi(ref[i])].first;
                        tlbLRU.push_front(atoi(ref[i]));
                        // printf("(Page %d, frame %d) brought into TLB \n", tlb[hasSpace].first, tlb[hasSpace].second);
                    }

                    else {
                        // TLB doesn't have space
                        // get victim 
                        int victim = tlbLRU.back();
                        tlbLRU.pop_back();
                        tlbLRU.push_front(atoi(ref[i]));

                        // remove the victim
                        for(int z =0;z<s;z++)
                        {
                            if (tlb[z].first == victim)
                            {
                                // printf("(Page %d, frame %d) was eliminated from TLB\n",tlb[z].first, tlb[z].second );
                                tlb[z].first = atoi(ref[i]);
                                tlb[z].second = pt[atoi(ref[i])].first;
                                // printf("(Page %d, frame %d) brought into TLB \n", tlb[z].first, tlb[z].second);
                                break;
                            }
                        }
                    }
                }
                else
                {
                    // page is not in the main memory
                    // have to bring into the main memory
                    printf("Process %d: for page reference %d, TLB miss -> with page fault.\n", *t, atoi(ref[i]));
                    // is the main memory full?
                    int mainFree = -1;
                    for (int z=0;z<f;z++)
                    {
                        if (main_mem[z] == -1)
                        {
                            mainFree = z;
                            break;
                        }
                    }
                    if (mainFree > -1)
                    {
                        // main memory is free
                        // directly add the frame
                        pt[atoi(ref[i])].first = mainFree;
                        pt[atoi(ref[i])].second = 0;
                        main_mem[mainFree] = atoi(ref[i]);
                        ptLRU.push_front(atoi(ref[i]));
                        // printf("TLB miss. Page %d not in main mem. Brought into frame %d\n",atoi(ref[i]),  mainFree);
                        // get this frame into the TLB
                        int hasSpace = -1;
                        for(int k=0;k<s;k++)
                        {
                            if (tlb[k].first == -1)
                            {
                                hasSpace = k;
                                break;
                            }
                        }

                        if (hasSpace>-1)
                        {
                            // tlb has space add directly
                            tlb[hasSpace].first = atoi(ref[i]);
                            tlb[hasSpace].second = pt[atoi(ref[i])].first;
                            tlbLRU.push_front(atoi(ref[i]));
                            // printf("(Page %d, frame %d) brought into TLB \n", tlb[hasSpace].first, tlb[hasSpace].second);
                        }

                        else {
                            // TLB doesn't have space
                            // get victim 
                            int victim = tlbLRU.back();
                            tlbLRU.pop_back();
                            tlbLRU.push_front(atoi(ref[i]));

                            // remove the victim
                            for(int z =0;z<s;z++)
                            {
                                if (tlb[z].first == victim)
                                {                            
                                    // printf("(Page %d, frame %d) was eliminated from TLB\n",tlb[z].first, tlb[z].second );
                                    tlb[z].first = atoi(ref[i]);
                                    tlb[z].second = pt[atoi(ref[i])].first;
                                    // printf("(Page %d, frame %d) brought into TLB \n", tlb[z].first, tlb[z].second);
                                    break;
                                }
                            }
                        }

                    }
                    else {
                        // main memory is not free
                        // get a victim
                        int victim = ptLRU.back();
                        printf("Victim page: %d\n", victim);
                        ptLRU.pop_back();
                        ptLRU.push_front(atoi(ref[i]));

                        // replace the victim
                        main_mem[pt[victim].first] =  atoi(ref[i]);
                        pt[victim].second = -1; // victim is no longer in main memory
                        // printf("(Page %d, frame %d) was eliminated\n", victim, pt[victim].first);
                        pt[atoi(ref[i])].first = pt[victim].first;  // frame number of victim
                        pt[atoi(ref[i])].second = 0;
                        // printf("TLB miss. Page %d not in main mem. Brought into frame %d\n", atoi(ref[i]), pt[victim].first);
                    
                        // check if victim was in the TLB
                        int victimInTLB = -1;
                        for(int z=0;z<s;z++)
                        {
                            if(tlb[z].first == victim)
                            {
                                // victim found.
                                // replace it
                                // printf("(Page %d, frame %d) was eliminated from TLB\n",tlb[z].first, tlb[z].second );
                                tlb[z].first = atoi(ref[i]);
                                tlb[z].second = pt[atoi(ref[i])].first;
                                // printf("(Page %d, frame %d) brought into TLB \n", tlb[z].first, tlb[z].second);
                                
                                tlbLRU.remove(victim);
                                tlbLRU.push_front(atoi(ref[i]));
                                victimInTLB = 0;
                                break;
                            }
                        }
                        
                        if (victimInTLB==-1)
                        {
                            // victim was not in TLB
                            // find a victim to get the last accessed value
                            // into the TLB

                            int victim = tlbLRU.back();
                            tlbLRU.pop_back();
                            tlbLRU.push_front(atoi(ref[i]));

                            // remove the victim
                            for(int z =0;z<s;z++)
                            {
                                if (tlb[z].first == victim)
                                {
                                    // printf("(Page %d, frame %d) was eliminated from TLB\n",tlb[z].first, tlb[z].second );
                                    tlb[z].first = atoi(ref[i]);
                                    tlb[z].second = pt[atoi(ref[i])].first;
                                    // printf("(Page %d, frame %d) brought into TLB\n", tlb[z].first, tlb[z].second);
                                    break;
                                }
                            }
                        }

                        else {
                            // victim was in TLB 
                            // job is already taken care of
                        }
                    }
                    faults += 1;
                }
            }
        }
        printf("Process %d: End of execution. Faults incurred: %d\n", *t, faults);
    }
}