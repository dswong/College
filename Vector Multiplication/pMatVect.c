
/* 
 * pMatVect - performs matrix multiplication of a MxN matrix with x vector with 
 * custom values. Prints first 50 elements of matrix and first 50 elements of result 
 * vector along with measured parallel time. 
 * 
 * Program takes in four command line arguments:
 * m = m dimension of MxN matrix
 * n = n dimension of MxN matrix
 * p = number of processes to be used
 * v = version 1 or 2,
 *	1: Uses padding
 *	2: Uses private variables
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* Global variables */
int     thread_count;
int     m, n;
double* A;
double* x;
double* y;

/* Parallel function */
void *Pth_mat_vect(void* rank); //uses padding
void *Pth_mat_vectPV(void* rank); //uses private variable

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) 
{
   int v;
   long       thread;
   pthread_t* thread_handles;
   double start, finish;

   m = atoi(argv[1]);
   n = atoi(argv[2]);
   thread_count = atoi(argv[3]);
   v = atoi(argv[4]);
   thread_handles = malloc(thread_count*sizeof(pthread_t));

   //We assume cache line is 128 bytes, each double is 8 bytes, 
   //so each cache line holds 16 doubles, pad by 16
   A = malloc(m*n*16*sizeof(double));
   x = malloc(n*16*sizeof(double));
   y = malloc(m*16*sizeof(double));

   GET_TIME(start);
   
   int i,j,k,l,p;
   for (i=0;i<m*16;i+=16) //make MxN matrix
   {
       for (j=0;j<n*16;j+=16)
       {
           if (i==j)
           {
               A[i*n+j] = 0.0;
           }
           else
           {
               A[i*n+j] = -1.0/(double)n;
           }
       }
   }
   for (k=0;k<n*16;k+=16) //make X vector
   {
       x[k] = (double)(k/16)/(double)n;
   }
   
   printf("Matrix (first 50 elements): \n");
   if ((m*n)<50) //print first 50 elements of matrix
   {
       for(l=0;l<(m*n*16);l+=16)
       {
           printf("%6.3f ",A[l]);
       }
   }
   else
   {
       for (l=0;l<50*16;l+=16)
       {
           printf("%6.3f ",A[l]);
       }
   }

   if (v==1) //padding version
   {
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*) thread);
        }
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
   }
   else //private variable version
   {
       for (thread = 0; thread < thread_count; thread++)
        {
            pthread_create(&thread_handles[thread], NULL, Pth_mat_vectPV, (void*) thread);
        }
        for (thread = 0; thread < thread_count; thread++)
        {
            pthread_join(thread_handles[thread], NULL);
        }
   }
   
   printf("\nResult vector (first 50 elements): \n");
   if (m<50) //print first 50 elements of result vector
   {
       for (p=0;p<m*16;p+=16)
       {
           printf("%6.3f ",y[p]);
       }
   }
   else
   {
       for (p=0;p<50*16;p+=16)
       {
           printf("%6.3f ",y[p]);
       }
   }
   
   free(A);
   free(x);
   free(y);
   
   GET_TIME(finish);
   printf("\nTotal time: %e\n",finish-start);
   
   return 0;
} 

/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply an mxn matrix by an nx1 column vector
 * In arg:         rank
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void *Pth_mat_vect(void* rank) 
{
   long my_rank = (long) rank;
   int i, j;
   int local_m = m/thread_count; 
   int my_first_row = my_rank*local_m;
   int my_last_row = (my_rank+1)*local_m - 1;

   for (i = my_first_row*16; i <= my_last_row*16; i+=16) 
   {
      y[i] = 0.0;
      for (j = 0; j < n*16; j+=16)
      {
          y[i] += A[i*n+j]*x[j];
      }
   }
   return NULL;
} 

void *Pth_mat_vectPV(void* rank) 
{
   long my_rank = (long) rank;
   int i, j, k;
   int local_m = m/thread_count; 
   int my_first_row = my_rank*local_m;
   int my_last_row = (my_rank+1)*local_m - 1;
   double local_y[m*16];

   for (i = my_first_row*16; i <= my_last_row*16; i+=16) 
   {
      local_y[i] = 0.0;
      for (j = 0; j < n*16; j+=16)
      {
          local_y[i] += A[i*n+j]*x[j];
      }
   }
   
   for (k = my_first_row*16; k <= my_last_row*16; k+=16) //transfer local_y to y
   {
       y[k] = local_y[k];         
   }
   return NULL;
} 