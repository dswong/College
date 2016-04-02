/*   
 * threadMatMatrix - threadMatMult - Performs matrix multiplication of a NxN matrix with vectors X 
 * and C, where Y = A*X+C, X=Y for t iterations. Prints first 30 elements of final 
 * result in X vector along with measured parallel time using pthreads.
 * 
 *  Program takes 4 arguments - n t p v
 *  n = size of matrix NxN
 *  t = number of iterations for loop
 *  p = number of processes used
 *  v = version 1 or 2, 
 *	1: Uses condition variables
 *	2: Uses barrier function
 *  
 *  Code adapted for use from Chapter 4 code of Parallel Programming by Peter S. Pacheco
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* Global variables */
int     p; //thread count
int     n; //size of matrix
int     t; //number of iterations to run
double* A;
double* x;
double* c;
double* y;

int counter = 0; //declare shared mutex and condition variable for choice 1 
pthread_mutex_t mutex;
pthread_cond_t cond_var;
pthread_barrier_t barrier;
   
/* Serial functions */
void Gen_matrix(double A[], int n);
void Gen_vectorXC(double x[], double c[], int n);

/* Parallel function */
void *Pth_mat_vect(void* rank); //used in version 1 with condition variables
void *Pth_mat_vectBar(void* rank); //used in version 2 with Pthread barrier

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) 
{
   long       thread;
   pthread_t* thread_handles;
   int i, j, v;
   double start, finish;
  
   n = atoi(argv[1]);
   t = atoi(argv[2]);
   p = atoi(argv[3]);
   v = atoi(argv[4]);

   thread_handles = malloc(p*sizeof(pthread_t));
   A = malloc(n*n*sizeof(double));
   x = malloc(n*sizeof(double));
   c = malloc(n*sizeof(double));
   y = malloc(n*sizeof(double));
   
   GET_TIME(start);
   Gen_matrix(A, n);
   Gen_vectorXC(x, c, n);
   
   if (v==1) // version 1 with condition variables
   {
       pthread_mutex_init(&mutex, NULL); //initialize mutex for use
       pthread_cond_init(&cond_var, NULL); //initialize condition variable
       for (thread = 0; thread < p; thread++)
       {
          pthread_create(&thread_handles[thread], NULL, Pth_mat_vect, (void*) thread);
       }
       for (thread = 0; thread < p; thread++)
       {
           pthread_join(thread_handles[thread], NULL);
       }
       pthread_mutex_destroy(&mutex); //We've finished with the mutex, so destroy it
       pthread_cond_destroy(&cond_var); //We've finished using the cond var, so destroy
   }
   else // version 2 with Pthread barrier function
   {
       pthread_barrier_init(&barrier, NULL, p); //initialize barrier 
       for (thread = 0; thread < p; thread++)
       {
          pthread_create(&thread_handles[thread], NULL, Pth_mat_vectBar, (void*) thread);
       }
       for (thread = 0; thread < p; thread++)
       {
           pthread_join(thread_handles[thread], NULL);
       }
       pthread_barrier_destroy(&barrier); //destroy barrier variable
   }

   if (n < 30)
   {
       for (j = 0; j < n; j++)
       {
           printf("%6.3f ",x[j]);
       }
   }
   else
   {
       for (j = 0; j < 30; j++)
       {
           printf("%6.3f ",x[j]);
       }
   }
   printf("\n");
   
   free(thread_handles);
   free(A);
   free(x);
   free(c);
   free(y);
   
   GET_TIME(finish);
   printf("Total time: %e\n",finish-start);
   
   return 0;
}  

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Use the random number generator random to generate
 *    the entries in A
 * In args:  n
 * Out arg:  A
 */
void Gen_matrix(double A[], int n) 
{
   int i, j;
   for (i = 0; i < n; i++)
   {
      for (j = 0; j < n; j++)
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
}

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Generates values for x and c
 * In arg:   n , x, c
 * Out arg:  A
 */
void Gen_vectorXC(double x[], double c[], int n) 
{
   int i, j;
   for (i = 0; i < n; i++)
   {
       x[i] = 0.0;
   }
   for (j = 0; j < n; j++)
   {
       c[j] = (double)j/(double)n;
   }
}

/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiply A by X and add C
 * In arg:         rank
 * Global in vars: A, x, n, c, p
 * Global out var: y
 */
void *Pth_mat_vect(void* rank) 
{
   long my_rank = (long) rank;
   int i;
   int j;
   int k;
   int l;
   int local_n = n/p; 
   int my_first_row = my_rank*local_n;
   int my_last_row = my_first_row + local_n;
   
   for (k = 0; k < t; k++) // run t times
   {
       for (i = my_first_row; i < my_last_row; i++) 
       {
          y[i] = 0.0;
          for (j = 0; j < n; j++) 
          {
              y[i] += A[i*n+j] * x[j];
          }
          y[i] += c[i];
       }
       // When each thread has finished their local work, we need to barrier
       pthread_mutex_lock(&mutex);  
       counter++;
       if (counter == p)
       {
           for (l = 0; l < n; l++) //last thread to enter barrier has the responsibility of transferring y to x
           {
               x[l] = y[l];
           }
           counter = 0;
           pthread_cond_broadcast(&cond_var);
       }
       else
       {
           while (pthread_cond_wait(&cond_var, &mutex) != 0);
       }
       pthread_mutex_unlock(&mutex);
   }
   return NULL;
} 

void *Pth_mat_vectBar(void* rank) //second version of function that uses Pthreads barrier
{
   long my_rank = (long) rank;
   int i;
   int j;
   int k;
   int l;
   int local_n = n/p; 
   int my_first_row = my_rank*local_n;
   int my_last_row = my_first_row + local_n;
   
   for (k = 0; k < t; k++) // run t times
   {
       for (i = my_first_row; i < my_last_row; i++) 
       {
          y[i] = 0.0;
          for (j = 0; j < n; j++) 
          {
              y[i] += A[i*n+j] * x[j];
          }
          y[i] += c[i];
       }
       pthread_barrier_wait(&barrier); //barrier when all threads have finished calculating
       if (my_rank == 0) //zero thread has the responsibility of transferring y to x
       {
           for (l = 0; l < n; l++) 
           {
               x[l] = y[l];
           }
       }
       pthread_barrier_wait(&barrier); //barrier, we must wait until values are transferred before performing more calculations
   }
   return NULL;
} 