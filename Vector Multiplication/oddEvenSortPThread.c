/* 
 * oddEvenSort - performs odd even parallel sort on list of ints, adapts 
 * code from Chapter 3 of Parallel Programming by Peter S. Pacheco
 * command line arguments n(number of items in list), p(number of processes)
 * List will contain numbers from 0 to 1000
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "timer.h"

/* Global variables */
int     p; //thread count
int     n; //size of list
int*    list; //global list

int counter = 0; //declare shared mutex and condition variable for choice 1 
pthread_mutex_t mutex;
pthread_cond_t cond_var;

void *Sort(void* rank); //performs preliminary local sort, sets partner process, begins phases
void Odd_even_iter(int phase, int partner, long my_rank); //decides which processes call Merge
void Merge(int first, int last); //merge two local sections through insertion sort

int main(int argc, char** argv) 
{
    long       thread;
    pthread_t* thread_handles;
    int i,j,k;
    n = atoi(argv[1]); //read in n numbers
    p = atoi(argv[2]); //number of processes
    double start, finish;
    
    list = malloc(n*sizeof(int));
    thread_handles = malloc(p*sizeof(pthread_t));
    
    GET_TIME(start);
    
    srand(p);
    for (i=0;i<(n/4);i++) //0-250, quarter one
    {
        list[i] = rand()%250;
    }
    for (i=(n/4);i<(n/4)*2;i++) //250-500, quarter two
    {
        list[i] = rand()%250+250;
    }
    for (i=(n/4)*2;i<(n/4)*3;i++) //500-750, quarter three
    {
        list[i] = rand()%250+500;
    }
    for (i=(n/4)*3;i<n;i++) //750-1000, quarter four
    {
        list[i] = rand()%250+750;
    }
    
    printf("Unsorted Global: \n");
    if (n<50) //print original list
    {
        for (j=0;j<n;j++)
        {
            printf("%d ",list[j]);
        }
    }
    else
    {
        for (j=0;j<50;j++) 
        {
            printf("%d ",list[j]);
        }
    }
    
    pthread_mutex_init(&mutex, NULL); //initialize mutex for use
    pthread_cond_init(&cond_var, NULL); //initialize condition variable
    for (thread = 0; thread < p; thread++)
    {       
        pthread_create(&thread_handles[thread], NULL, Sort, (void*) thread);       
    }
    for (thread = 0; thread < p; thread++)
    {
        pthread_join(thread_handles[thread], NULL);
    }        
    pthread_mutex_destroy(&mutex); //We've finished with the mutex, so destroy it
    pthread_cond_destroy(&cond_var); //We've finished using the cond var, so destroy

    printf("\nSorted Global: \n");
    if (n<50) //print sorted list
    {
        for (k=0;k<n;k++)
        {
            printf("%d ",list[k]);
        }
    }
    else
    {
        for (k=0;k<50;k++) 
        {
            printf("%d ",list[k]);
        }
    }
    
    free(thread_handles);
    free(list);
    
    GET_TIME(finish);
    printf("\nTotal Time = %e\n", finish-start);

    return 0;
}


void *Sort(void* rank) 
{
   long my_rank = (long) rank;
   int phase;
   int partner;
   
   // Find partners, each process's partner is the next process
   partner = my_rank+1;
   if (partner == p) //last process in line, so do nothing during phases
   {
       partner = -1;
   }
   
   int start = my_rank*(n/p);
   int end = start+(n/p);
   int flag = 1; //assumes list is already sorted
   int i;
   for (i=start+1;i<end;i++)
   {
       if (list[i-1] > list[i])
       {
           flag = 0; //list is not sorted
       }
   }
   
   if (flag == 0) //list is not sorted, use insertion sort since local assigned portion is substantially sorted
   {
       int j, k;
       int temp;
       for (j=start+1;j<end;j++)
       {
           temp = list[j];
           for (k = j; k > start && temp < list[k-1]; k--)
           {
               list[k] = list[k-1];
           }
           list[k] = temp;
       }
   }

   int l;
   for (phase=0;phase<p;phase++)
   {
       Odd_even_iter(phase, partner, my_rank);
	//barrier to prevent overwrite between phases
       pthread_mutex_lock(&mutex);  
       counter++;
       if (counter == p)
       {
           counter = 0;
           pthread_cond_broadcast(&cond_var);
       }
       else
       {
           while (pthread_cond_wait(&cond_var, &mutex) != 0);
       }
       pthread_mutex_unlock(&mutex);
   }
}  


void Odd_even_iter(int phase, int partner, long my_rank)
{
    int my_first = my_rank*(n/p); // initialize local and partner indexes in global list
    int my_last = (my_first+(n/p))-1;
    int partner_first = my_last+1;
    int partner_last = partner_first+(n/p); //points to section limit
    
    if (phase % 2 == 0 && my_rank % 2 == 0) //only even processes do work
    {
      if (partner != -1) 
      {
         if (list[my_last]>=list[partner_first]) //swap only if local elements are greater
         {
            Merge(my_first,partner_last);
         } 
      }
    } 
    
    if (phase %2 != 0 && my_rank % 2 != 0) //odd phase, only odd processes do work
    {
      if (partner != -1) 
      {
         if (list[my_last]>=list[partner_first]) //swap only if local elements are greater
         {
            Merge(my_first,partner_last);
         }
      }
    }
} 


void Merge(int first, int last)
{
    int i, j, temp;
    for (i = first+1; i < last; i++)
    {
        temp = list[i];
        for (j = i; j > first && temp < list[j-1]; j--)
        {
            list[j] = list[j-1];
        }
        list[j] = temp;
    }
} 



