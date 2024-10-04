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

double* combineSortedArrays(double* left, double* right, int leftSize, int rightSize) {
   double* returnArray = new double[leftSize+rightSize];
   int r = 0;
   int l = 0;
   for(int i = 0; i < leftSize+rightSize; i++){
    if(r == rightSize) {
      returnArray[i] = left[l];
      l++;
    }
    else if(l == leftSize) {
      returnArray[i] = right[r];
      r++;
    }
    else if(right[r] < left[l]) {
      returnArray[i] = right[r];
      r++;
    }
    else {
      returnArray[i] = left[l];
      l++;
    }
  }
  return returnArray;
}

double* mergeSort(double* arr, int size) {
   if(size == 1) {
      return arr;
   } else {
      int leftSize = size/2;
      int rightSize = size - leftSize;
      double* leftSorted = mergeSort(arr, leftSize);
      double* rightSorted = mergeSort(arr + leftSize, rightSize);
      return combineSortedArrays(leftSorted, rightSorted, leftSize, rightSize);
   }
}

double* startChildProcesses(int id, int childCount, double* arr, int arrSize) {
   //printf("process %d, children %d\n", id, childCount);
   int leftSize = arrSize/2;
   int rightSize = arrSize - leftSize;
   MPI_Status status;
   if(childCount == 0) {
      return mergeSort(arr, arrSize);
   } else if(childCount == 1) {
      //send the left half of the array and its size to child for sorting
      MPI_Send(&leftSize, 1, MPI_INT, id+1, FROM_PARENT, MPI_COMM_WORLD);
      MPI_Send(arr, leftSize, MPI_DOUBLE, id+1, FROM_PARENT, MPI_COMM_WORLD);

      
      //sort the right half of the array ourselves
      double* rightSorted = mergeSort(arr+leftSize, rightSize);

      //receive the sorted left half of the array from child
      double leftSorted[leftSize];
      //printf("%d getting left half from child %d\n", id, id+1);
      MPI_Recv(&leftSorted, leftSize, MPI_DOUBLE, id+1, FROM_CHILD, MPI_COMM_WORLD, &status);

      //combine and return the arrays
      double* returnArr = combineSortedArrays(leftSorted, rightSorted, leftSize, rightSize);
      //printf("Process %d returning sorted array: ", id);
      //printArray(returnArr, arrSize);
      delete[] rightSorted;
      return returnArr;
   } else { //2 children
      //mpi send left half to child
      //mpi send right half to child
      double* leftSorted;
      double* rightSorted;
      //mpi recv left half from child
      //mpi recv right half from child
      return combineSortedArrays(leftSorted, rightSorted, leftSize, rightSize);
   }
}

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
   int boredProcesses;
   double* sortedArr;

   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

   for(int n = 0; n < numtasks; n++){
      if(taskid == n) {
         if(taskid == MASTER) { //only our first array initializes the array to be sorted
            boredProcesses = numtasks-1;
            int children = std::min(boredProcesses, 2);
            boredProcesses -= children;

            double* toSort = generateArray(sizeOfArray, inputType);
            printf("****************************\nINITIAL ARRAY: \n");
            printArray(toSort, sizeOfArray);
            printf("****************************\n\n");

            if(children == 2)
               printf("Process %d sending data to processes %d, %d\n", n, n+1, n+2);
            else if(children == 1)
               printf("Process %d sending data to process %d\n", n, n+1);
            else if(children == 0)
               printf("Process %d is leaf node; Calculating...\n", n);

            //send this to the next thread
            if(n + 1 < numtasks)
               MPI_Send(&boredProcesses, 1, MPI_INT, n + 1, FROM_PARENT, MPI_COMM_WORLD);

            sortedArr = startChildProcesses(n, children, toSort, sizeOfArray);
         } else {
            //get bored processes so we can compute child number
            MPI_Recv(&boredProcesses, 1, MPI_INT, n-1, FROM_PARENT, MPI_COMM_WORLD, &status);
            int children = std::min(boredProcesses, 2);
            boredProcesses -= children;

            if(children == 2)
               printf("Process %d sending data to processes %d, %d\n", n, n+1, n+2);
            else if(children == 1)
               printf("Process %d sending data to process %d\n", n, n+1);
            else if(children == 0)
               printf("Process %d is leaf node; Calculating...\n", n);

            //send this to the next thread
            if(n + 1 < numtasks)
               MPI_Send(&boredProcesses, 1, MPI_INT, n + 1, FROM_PARENT, MPI_COMM_WORLD);

            int parentId = n - ((n+1)%2) - 1; //n-1 if odd, n-2 if even
            int toSortSize;

            //get array and its size from parent process
            MPI_Recv(&toSortSize, 1, MPI_INT, parentId, FROM_PARENT, MPI_COMM_WORLD, &status);
            double toSort[toSortSize];
            MPI_Recv(&toSort, toSortSize, MPI_DOUBLE, parentId, FROM_PARENT, MPI_COMM_WORLD, &status);

            //start child processes and receive sorted array
            double* sorted = startChildProcesses(n, children, toSort, toSortSize);

            //send sorted array back to parent process; they already know the size
            MPI_Send(sorted, toSortSize, MPI_DOUBLE, parentId, FROM_CHILD, MPI_COMM_WORLD);
         }
      }
   }

   if(taskid == MASTER) {
      printf("\n****************************\nFINAL ARRAY: \n");
      printArray(sortedArr, sizeOfArray);
      printf("****************************\n");
   }

   MPI_Finalize();
}