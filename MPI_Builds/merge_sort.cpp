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
#include <adiak.hpp>

#define MASTER 0               /* taskid of first task */
#define FROM_PARENT 1          /* setting a message type */
#define FROM_CHILD 2          /* setting a message type */

const char* main_cali = "main";
const char* data_init_runtime = "data_init_runtime";
const char* correctness_check = "correctness_check";
const char* comm = "comm";
const char* comm_small = "comm_small";
const char* comm_large = "comm_large";
const char* comp = "comp";
const char* comp_small = "comp_small";
const char* comp_large = "comp_large";

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

/*int getChildCount(int id, int totalWorkers) {
   int leftChild = 2*id+1;
   if(leftChild + 1 < totalWorkers) {
      return 2;
   } else if(leftChild < totalWorkers) {
      return 1;
   } else {
      return 0;
   }
}*/

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

/*double* startChildProcesses(int id, int childCount, double* arr, int arrSize) {
   CALI_MARK_BEGIN(comp);
   CALI_MARK_BEGIN(comp_small);
   int leftSize = arrSize/2;
   int rightSize = arrSize - leftSize;
   MPI_Status status;
   double* returnArr;
   int firstChildId = 2*id+1;
   if(childCount == 0) {
      CALI_MARK_END(comp_small);
      CALI_MARK_BEGIN(comp_large);
      returnArr = mergeSort(arr, arrSize);
      CALI_MARK_END(comp_large);
      CALI_MARK_END(comp);
      //printArray(returnArr, arrSize);
      return returnArr;
   } else if(childCount == 1) {
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);
      //send the left half of the array and its size to child for sorting
      CALI_MARK_BEGIN(comm);
      CALI_MARK_BEGIN(comm_small);
      MPI_Send(&leftSize, 1, MPI_INT, firstChildId, FROM_PARENT, MPI_COMM_WORLD);
      CALI_MARK_END(comm_small);
      CALI_MARK_BEGIN(comm_large);
      MPI_Send(arr, leftSize, MPI_DOUBLE, firstChildId, FROM_PARENT, MPI_COMM_WORLD);
      CALI_MARK_END(comm_large);
      CALI_MARK_END(comm);

      //sort the right half of the array ourselves
      CALI_MARK_BEGIN(comp);
      CALI_MARK_BEGIN(comp_large);
      double* rightSorted = mergeSort(arr+leftSize, rightSize);
      CALI_MARK_END(comp_large);
      CALI_MARK_END(comp);

      //receive the sorted left half of the array from child
      double leftSorted[leftSize];
      CALI_MARK_BEGIN(comm);
      CALI_MARK_BEGIN(comm_large);
      MPI_Recv(&leftSorted, leftSize, MPI_DOUBLE, firstChildId, FROM_CHILD, MPI_COMM_WORLD, &status);
      CALI_MARK_END(comm_large);
      CALI_MARK_END(comm);

      //combine and return the arrays
      CALI_MARK_BEGIN(comp);
      CALI_MARK_BEGIN(comp_large);
      returnArr = combineSortedArrays(leftSorted, rightSorted, leftSize, rightSize);
      CALI_MARK_END(comp_large);
      CALI_MARK_END(comp);
      //printf("Process %d sorted array: ", id);
      //printArray(returnArr, arrSize);

      delete[] rightSorted;
      return returnArr;
   } else { //2 children
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);

      //send the left half of the array and its size to child for sorting
      CALI_MARK_BEGIN(comm);
      CALI_MARK_BEGIN(comm_small);
      MPI_Send(&leftSize, 1, MPI_INT, firstChildId, FROM_PARENT, MPI_COMM_WORLD);
      CALI_MARK_END(comm_small);
      CALI_MARK_BEGIN(comm_large);
      MPI_Send(arr, leftSize, MPI_DOUBLE, firstChildId, FROM_PARENT, MPI_COMM_WORLD);
      CALI_MARK_END(comm_large);

      //send the right half of the array and its size to child for sorting
      CALI_MARK_BEGIN(comm_small);
      MPI_Send(&rightSize, 1, MPI_INT, firstChildId+1, FROM_PARENT, MPI_COMM_WORLD);
      CALI_MARK_END(comm_small);
      CALI_MARK_BEGIN(comm_large);
      MPI_Send(arr + leftSize, rightSize, MPI_DOUBLE, firstChildId+1, FROM_PARENT, MPI_COMM_WORLD);
      CALI_MARK_END(comm_large);
      CALI_MARK_END(comm);

      //receive the sorted arrays
      CALI_MARK_BEGIN(comm);
      CALI_MARK_BEGIN(comm_large);
      double leftSorted[leftSize];
      double rightSorted[rightSize];
      MPI_Recv(&leftSorted, leftSize, MPI_DOUBLE, firstChildId, FROM_CHILD, MPI_COMM_WORLD, &status);
      MPI_Recv(&rightSorted, leftSize, MPI_DOUBLE, firstChildId+1, FROM_CHILD, MPI_COMM_WORLD, &status);
      CALI_MARK_END(comm_large);
      CALI_MARK_END(comm);

      //combine and return the arrays
      CALI_MARK_BEGIN(comp);
      CALI_MARK_BEGIN(comp_large);
      returnArr = combineSortedArrays(leftSorted, rightSorted, leftSize, rightSize);
      CALI_MARK_END(comp_large);
      CALI_MARK_END(comp);
      //printf("Process %d sorted array: ", id);
      //printArray(returnArr, arrSize);
      
      return returnArr;
   }
}*/

void printIntro(int numtasks, int sizeOfArray, int inputType) {
   if(inputType == RANDOM_INPUT)
      printf("Starting merge sort with %d processes, array size %d, random initial data\n", numtasks, sizeOfArray);
   if(inputType == SORTED_INPUT)
      printf("Starting merge sort with %d processes, array size %d, sorted initial data\n", numtasks, sizeOfArray);
   if(inputType == REVERSE_SORTED_INPUT)
      printf("Starting merge sort with %d processes, array size %d, reverse sorted initial data\n", numtasks, sizeOfArray);
   if(inputType == ONE_PERC_PERMUTED)
      printf("Starting merge sort with %d processes, array size %d, 1%% permuted initial data\n", numtasks, sizeOfArray);
}

int main (int argc, char *argv[])
{
   int sizeOfArray; //this is an input to the code
   int inputType;
   std::string inputString;
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
   int twoKids;
   double* sortedArr;
   double whole_computation_time;

   if(inputType == RANDOM_INPUT)
      inputString = "random";
   if(inputType == SORTED_INPUT)
       inputString = "sorted";
   if(inputType == REVERSE_SORTED_INPUT)
       inputString = "reversed";
   if(inputType == ONE_PERC_PERMUTED)
       inputString = "permuted";

   // Create caliper ConfigManager object
   cali::ConfigManager mgr;
   mgr.start();

   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
   MPI_Comm_size(MPI_COMM_WORLD,&numtasks);

   if(taskid == MASTER) {
      printIntro(numtasks, sizeOfArray, inputType);
   }

   CALI_MARK_BEGIN(main_cali);
   double wholeMasterStart = MPI_Wtime();

   CALI_MARK_BEGIN(data_init_runtime);
   int chunkSize = sizeOfArray/numtasks; //we know that both proc num and size will be powers of 2, so every process will always get an array of the same size
   double* arrChunk = generateArray(chunkSize, inputType); //although she mentioned sending elements across permuations, this is no different computationally than permuting the elements within an array
   CALI_MARK_END(data_init_runtime);

   /*
   if(taskid == MASTER) {
      printIntro(numtasks, sizeOfArray, inputType);
      CALI_MARK_BEGIN(data_init_runtime);
      toSort = generateArray(sizeOfArray, inputType);
      CALI_MARK_END(data_init_runtime);
   }

   CALI_MARK_BEGIN(comp);
   CALI_MARK_BEGIN(comp_small);
   int chunkSize = sizeOfArray/numtasks;
   int remainder = sizeOfArray%numtasks;
   CALI_MARK_END(comp_small);
   CALI_MARK_END(comp);

   double* arrChunk = new double[chunkSize + remainder]; //in case we have extra elements we have room for them

   //scatter a chunk of the array to each task
   CALI_MARK_BEGIN(comm);
   CALI_MARK_BEGIN(comm_large);
   MPI_Scatter(toSort, chunkSize, MPI_DOUBLE, arrChunk, chunkSize, MPI_DOUBLE, 0, MPI_COMM_WORLD); //todo not divisible
   CALI_MARK_END(comm_large);
   CALI_MARK_END(comm);

   //put the remainder into the master buffer
   CALI_MARK_BEGIN(comp);
   CALI_MARK_BEGIN(comp_large);
   if(remainder != 0 && taskid == MASTER) {
      for(int i = 0; i < remainder; i++) {
         arrChunk[chunkSize+i] = toSort[chunkSize*numtasks+i];
      }
      chunkSize += remainder;
   }
   CALI_MARK_END(comp_large);
   CALI_MARK_END(comp);*/

   CALI_MARK_END(main_cali);
   printf("Process %d sorting chunk...\n", taskid);
   CALI_MARK_BEGIN(main_cali);

   CALI_MARK_BEGIN(comp);
   CALI_MARK_BEGIN(comp_large);
   arrChunk = mergeSort(arrChunk, chunkSize);
   CALI_MARK_END(comp_large);

   CALI_MARK_BEGIN(comp_small);
   //if we are not one of the last 2 processes, e.g. we have a "false" parent
   if(taskid < numtasks-2) {
      int falseParent = ((taskid+numtasks) + (taskid+numtasks)%2 - 2)/2;
      int mergeParent = (taskid + taskid%2 - 2)/2; //who you send your final merged array to
      int leftChild = (2*taskid+1)%numtasks;
      int rightChild = (leftChild+1)%numtasks;
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);

      //send false parent sorted array and its size
      CALI_MARK_BEGIN(comm);
      CALI_MARK_BEGIN(comm_small);
      MPI_Send(&chunkSize, 1, MPI_INT, falseParent, FROM_CHILD, MPI_COMM_WORLD);
      CALI_MARK_END(comm_small);
      CALI_MARK_BEGIN(comm_large);
      MPI_Send(arrChunk, chunkSize, MPI_DOUBLE, falseParent, FROM_CHILD, MPI_COMM_WORLD);
      CALI_MARK_END(comm_large);

      //wait to recv from your 2 children
      CALI_MARK_BEGIN(comm_small);
      int leftSize;
      int rightSize;
      MPI_Recv(&leftSize, 1, MPI_INT, leftChild, FROM_CHILD, MPI_COMM_WORLD, &status);
      MPI_Recv(&rightSize, 1, MPI_INT, rightChild, FROM_CHILD, MPI_COMM_WORLD, &status);
      CALI_MARK_END(comm_small);

      double* leftSorted = new double[leftSize];
      double* rightSorted = new double[rightSize];
      
      CALI_MARK_BEGIN(comm_large);
      MPI_Recv(leftSorted, leftSize, MPI_DOUBLE, leftChild, FROM_CHILD, MPI_COMM_WORLD, &status);
      MPI_Recv(rightSorted, rightSize, MPI_DOUBLE, rightChild, FROM_CHILD, MPI_COMM_WORLD, &status);
      CALI_MARK_END(comm_large);
      CALI_MARK_END(comm);

      //merge the arrays you got together
      CALI_MARK_END(main_cali);
      printf("Process %d combining arrays from processes %d and %d\n", taskid, leftChild, rightChild);
      CALI_MARK_BEGIN(main_cali);

      CALI_MARK_BEGIN(comp);
      CALI_MARK_BEGIN(comp_large);
      double* combined = combineSortedArrays(leftSorted, rightSorted, leftSize, rightSize);
      CALI_MARK_END(comp_large);
      CALI_MARK_BEGIN(comp_small);
      int combinedSize = leftSize+rightSize;
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);

      //send real parent the merged array if you aren't the master task
      if(taskid != MASTER) {
         CALI_MARK_BEGIN(comm);
         CALI_MARK_BEGIN(comm_small);
         MPI_Send(&combinedSize, 1, MPI_INT, mergeParent, FROM_CHILD, MPI_COMM_WORLD);
         CALI_MARK_END(comm_small);
         CALI_MARK_BEGIN(comm_large);
         MPI_Send(combined, leftSize+rightSize, MPI_DOUBLE, mergeParent, FROM_CHILD, MPI_COMM_WORLD);
         CALI_MARK_END(comm_large);
         CALI_MARK_END(comm);
      } else {
         sortedArr = combined;
      }
   } else if(taskid == numtasks-2) { //second to last, has one child
      int leftChild = (2*taskid+1)%numtasks;
      int mergeParent = (taskid + taskid%2 - 2)/2;
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);

      //recv from your child
      int leftSize;
      CALI_MARK_BEGIN(comm);
      CALI_MARK_BEGIN(comm_small);
      MPI_Recv(&leftSize, 1, MPI_INT, leftChild, FROM_CHILD, MPI_COMM_WORLD, &status);
      CALI_MARK_END(comm_small);

      double* leftSorted = new double[leftSize];
      CALI_MARK_BEGIN(comm_large);
      MPI_Recv(leftSorted, leftSize, MPI_DOUBLE, leftChild, FROM_CHILD, MPI_COMM_WORLD, &status);
      CALI_MARK_END(comm_large);
      CALI_MARK_END(comm);

      //merge your array with child array
      CALI_MARK_END(main_cali);
      printf("Process %d combining arrays from processes %d and %d\n", taskid, taskid, leftChild);
      CALI_MARK_BEGIN(main_cali);

      CALI_MARK_BEGIN(comp);
      CALI_MARK_BEGIN(comp_large);
      double* combined = combineSortedArrays(leftSorted, arrChunk, leftSize, chunkSize);
      CALI_MARK_END(comp_large);
      CALI_MARK_BEGIN(comp_small);
      int combinedSize = leftSize+chunkSize;
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);

      //send real parent the merged array if you aren't the master task
      if(taskid != MASTER) {
         CALI_MARK_BEGIN(comm);
         CALI_MARK_BEGIN(comm_small);
         MPI_Send(&combinedSize, 1, MPI_INT, mergeParent, FROM_CHILD, MPI_COMM_WORLD);
         CALI_MARK_END(comm_small);
         CALI_MARK_BEGIN(comm_large);
         MPI_Send(combined, leftSize+chunkSize, MPI_DOUBLE, mergeParent, FROM_CHILD, MPI_COMM_WORLD);
         CALI_MARK_END(comm_large);
         CALI_MARK_END(comm);
      } else {
         sortedArr = combined;
      }
   } else { //last element, leaf node
      int mergeParent = (taskid + taskid%2 - 2)/2;
      CALI_MARK_END(comp_small);
      CALI_MARK_END(comp);

      //send real parent the merged array if you aren't the master task
      if(taskid != MASTER) {
         CALI_MARK_BEGIN(comm);
         CALI_MARK_BEGIN(comm_small);
         MPI_Send(&chunkSize, 1, MPI_INT, mergeParent, FROM_CHILD, MPI_COMM_WORLD);
         CALI_MARK_END(comm_small);
         CALI_MARK_BEGIN(comm_large);
         MPI_Send(arrChunk, chunkSize, MPI_DOUBLE, mergeParent, FROM_CHILD, MPI_COMM_WORLD);
         CALI_MARK_END(comm_large);
         CALI_MARK_END(comm);
      } else {
         sortedArr = arrChunk;
      }
   }


   /*for(int n = 0; n < numtasks; n++){
      if(taskid == n) {
         if(taskid == MASTER) { //only our first array initializes the array to be sorted
            CALI_MARK_END(main_cali);
            printIntro(numtasks, sizeOfArray, inputType);
            CALI_MARK_BEGIN(main_cali);

            CALI_MARK_BEGIN(comp);
            CALI_MARK_BEGIN(comp_small);
            int children = getChildCount(n, numtasks+twoKids);
            CALI_MARK_END(comp_small);
            CALI_MARK_END(comp);

            CALI_MARK_BEGIN(data_init_runtime);
            double* toSort = generateArray(sizeOfArray, inputType);
            CALI_MARK_END(data_init_runtime);
            //printf("****************************\nINITIAL ARRAY: \n");
            //printArray(toSort, sizeOfArray);
            //printf("****************************\n\n");
            CALI_MARK_END(main_cali);
            if(children == 2)
               printf("Process %d sending data to processes %d, %d\n", n, n+1, n+2);
            else if(children == 1)
               printf("Process %d sending data to process %d\n", n, n+1);
            else if(children == 0)
               printf("Process %d is leaf node; Calculating...\n", n);

            CALI_MARK_BEGIN(main_cali);
            sortedArr = startChildProcesses(n, children, toSort, sizeOfArray);
         } else {
            CALI_MARK_BEGIN(comp);
            CALI_MARK_BEGIN(comp_small);
            int children = getChildCount(n, numtasks+twoKids);
            int leftChild = (2*n+1)%numtasks;
            
            int parentId = (n + n%2 - 2)/2; //(n-1)/2 if odd, (n-2)/2 if even
            int toSortSize;
            CALI_MARK_END(comp_small);
            CALI_MARK_END(comp);
            
            //get array and its size from parent process
            CALI_MARK_BEGIN(comm);
            CALI_MARK_BEGIN(comm_small);
            MPI_Recv(&toSortSize, 1, MPI_INT, parentId, FROM_PARENT, MPI_COMM_WORLD, &status);
            CALI_MARK_END(comm_small);
            CALI_MARK_END(comm);

            double toSort[toSortSize];

            CALI_MARK_BEGIN(comm);
            CALI_MARK_BEGIN(comm_large);
            MPI_Recv(&toSort, toSortSize, MPI_DOUBLE, parentId, FROM_PARENT, MPI_COMM_WORLD, &status);
            CALI_MARK_END(comm_large);
            CALI_MARK_END(comm);

            CALI_MARK_END(main_cali);
            //print some info
            if(children == 2)
               printf("Process %d sending data to processes %d, %d\n", n, leftChild, (leftChild+1)%numtasks);
            else if(children == 1)
               printf("Process %d sending data to process %d\n", n, leftChild);
            else if(children == 0)
               printf("Process %d is leaf node; Calculating...\n", n);

            CALI_MARK_BEGIN(main_cali);
            //start child processes and receive sorted array
            double* sorted = startChildProcesses(n, children, toSort, toSortSize);
            //send sorted array back to parent process; they already know the size
            CALI_MARK_BEGIN(comm);
            CALI_MARK_BEGIN(comm_large);
            MPI_Send(sorted, toSortSize, MPI_DOUBLE, parentId, FROM_CHILD, MPI_COMM_WORLD);
            CALI_MARK_END(comm_large);
            CALI_MARK_END(comm);
         }
      }
   }*/

   if(taskid == MASTER) {
      CALI_MARK_BEGIN(correctness_check);
      if(!verifySorted(sortedArr, sizeOfArray)) {
         CALI_MARK_END(correctness_check);
         CALI_MARK_END(main_cali);
         printf("There is a bug! This final array is not sorted.\n");
         exit(-1);
      }
      CALI_MARK_END(correctness_check);
      CALI_MARK_END(main_cali); //moved this here so only the master task can end it/we get one time
   }

   double wholeMasterEnd = MPI_Wtime();

   if(taskid == MASTER) {
      printf("Total time taken: %f\n", wholeMasterEnd-wholeMasterStart);
   }

   adiak::init(NULL);
   adiak::launchdate();    // launch date of the job
   adiak::libraries();     // Libraries used
   adiak::cmdline();       // Command line used to launch the job
   adiak::clustername();   // Name of the cluster
   adiak::value("algorithm", "merge"); // The name of the algorithm you are using (e.g., "merge", "bitonic")
   adiak::value("programming_model", "mpi"); // e.g. "mpi"
   adiak::value("data_type", "double"); // The datatype of input elements (e.g., double, int, float)
   adiak::value("size_of_data_type", sizeof(double)); // sizeof(datatype) of input elements in bytes (e.g., 1, 2, 4)
   adiak::value("input_size", sizeOfArray); // The number of elements in input dataset (1000)
   adiak::value("input_type", inputString); // For sorting, this would be choices: ("Sorted", "ReverseSorted", "Random", "1_perc_perturbed")
   adiak::value("num_procs", numtasks); // The number of processors (MPI ranks)
   adiak::value("scalability", "weak"); // The scalability of your algorithm. choices: ("strong", "weak")
   adiak::value("group_num", 1); // The number of your group (integer, e.g., 1, 10)
   adiak::value("implementation_source", "handwritten"); // Where you got the source code of your algorithm. choices: ("online", "ai", "handwritten").


   mgr.stop();
   mgr.flush();

   MPI_Finalize();
}
