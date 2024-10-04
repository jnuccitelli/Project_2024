/******************************************************************************
* FILE: merge_sort.cpp
* DESCRIPTION:  
*   Merge Sort Algorithm implementation
*   In this code, tasks spawn 0-2 child processes to process half 
*   of an array using merge sort, and then combine the result all the
*   wau up to output an array sorted least to greatest
* AUTHOR: Ariela Mitrani
* LAST REVISED: 10/04/2024
******************************************************************************/
#include "mpi.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <caliper/cali.h> //for performance analysis
#include <caliper/cali-manager.h>

#define MASTER 0               /* taskid of first task */
#define FROM_PARENT 1          /* setting a message type */
#define FROM_CHILD 2          /* setting a message type */

int main (int argc, char *argv[])
{
   int sizeOfArray; //this is an input to the code
   int inputType;
   if (argc == 3)
   {
       sizeOfArray = atoi(argv[1]);
       inputType = atoi(argv[2]);
       if(inputType < 0 || inputType > 3) {
         printf("\n Please provide a valid input type [0-3]");
       }
   }
   else
   {
       printf("\n Please provide arguments [sizeOfArray] [input type]");
       return 0;
   }
   int	numtasks,              /* number of tasks in partition */
   	taskid,                /* a task identifier */
   	source,                /* task id of message source */
   	dest,                  /* task id of message destination */
   	mtype;                 /* message type */
   MPI_Status status;

   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

   int boredProcesses = numtasks;
   for(int n = 0; n < numtasks; n++){
      if(taskid == n) {
         int children = std::min(boredProcesses, 2);
         boredProcesses -= children;

         if(taskid == MASTER) { //only our first array initializes the array to be sorted
            double* toSort = generateArray(sizeOfArray, inputType);
            printArray(toSort, sizeOfArray);
         }
      }
   }

   MPI_Finalize();
}