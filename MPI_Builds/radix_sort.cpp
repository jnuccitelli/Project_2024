/******************************************************************************
* FILE: radix_sort.cpp
* DESCRIPTION:  
*   Radix Sort Algorithm implementation
*   IPerforms parallel radix sort on an array
* AUTHOR: Alexander Nuccitelli
* LAST REVISED: 10/13/2024
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <caliper/cali.h> //for performance analysis
#include <caliper/cali-manager.h>
#include <adiak.hpp>

#define RANDOM_INPUT 0
#define SORTED_INPUT 1
#define REVERSE_SORTED_INPUT 2
#define ONE_PERC_PERMUTED 3

//HELPER FUNCTIONS FOR RADIX SORT
int* generateArray(int size, int inputType, int processorNumber, int totalProcesses) {
    srand(std::time(0)); //give rand a random seed so we get different answers each time
    int* arr = new int[size];

    //random input
    if(inputType == RANDOM_INPUT) {
        for(int i = 0; i < size; i++) {
            arr[i] = rand(); //generates random ints
        }
        return arr;
    } 

    //sorted input
    else if(inputType == SORTED_INPUT) {
        int lastVal = 0;
        for(int i = 0; i < size; i++) {
            int newVal = ((rand() % 100) / 10) + lastVal + (size * processorNumber * 10);
            arr[i] = newVal; //generates ints from the last value to 10+last value
            lastVal = newVal;
        }
        return arr;
    } 

    //reverse sorted input
    else if(inputType == REVERSE_SORTED_INPUT) {
        int lastVal = 0;
        for(int i = size-1; i >= 0; i--) {
            double newVal = ((rand() % 100) / 10) + lastVal + (size * (totalProcesses - processorNumber) * 10);
            arr[i] = newVal; 
            lastVal = newVal;
        }
        return arr;
    } 

    //one percent permuted
    else { //one percent permuted
        //create sorted array
        int lastVal = 0;
        for(int i = 0; i < size; i++) {
            int newVal = ((rand() % 100) / 10) + lastVal + (size * processorNumber * 10);
            arr[i] = newVal; 
            lastVal = newVal;
        }

        //randomly update 1% of elements
        for (int i =0; i < size; ++i) {
            if (rand() % 100 == 0) {
                arra[i] = rand();
            }
        }

        return arr;
    }
}

void printArray(int* arr, int size, int processorNumber) {
    printf("Processor number %d, array:", processorNumber);
    for(int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

bool verifySorted(int* arr, int size) { //check if an array was sorted correctly
    int max = 0;
    for(int i = 0; i < size; i++) {
        if(arr[i] < max) {
            return false;
        }
        max = arr[i];
    }
    return true;
}

int main (int argc, char *argv[]) {

    int sizeOfArray; //this is an input to the code
    int inputType;

    if (argc == 3) {
        sizeOfArray = atoi(argv[1]);
        inputType = atoi(argv[2]);
        if(inputType < 0 || inputType > 3) {
            printf("\n Please provide a valid input type [0-3]");
        }
    }
    else {
        printf("\n Please provide arguments [sizeOfArray] [input type]");
        return 0;
    }

    int taskid;
    int numTasks;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numTasks);

    int* arr = generateArray(sizeOfArray, inputType, taskid, numTasks);
    printArray(arr, sizeOfArray, taskid);

    MPI_Finalize();
}
// int arraySize = user input for array size
// int procNum
// int taskId

// MPI_Init(&argc,&argv)
// MPI_Comm_rank(MPI_COMM_WORLD,&taskid)
// MPI_Comm_size(MPI_COMM_WORLD,&procNum)

// int totalArray[arraySize]

// //Generate the array
// for (i in procNum) {
//   int offset = arraySize / ProcNum * taskId
//   totalArray[from offset to (offset + (arraySize/ProcNum)] = Array generation
// }

// //sort the array
// for (i in numBits of type) {
//   int[arraySize / ProcNum] localArray = totalArray[from offset to (offset + (arraySize/ProcNum)]

//   //get total zeroes to all processes
//   int localNumZeroes
//   localArray, localNumZeroes = local_counting_sort(localArray, bitnumber = i)
//   int localNumOnes = size(localArray) - localNumZeroes
//   int totalNumZeroes
//   MPI_Reduce(reduce local_num_zeroes to total_num_zeroes to process 0)
//   if (taskId == 0) {
//     MPI_Send(totalNumZeroes)
//   }
//   else {
//     MPI_Recieve(totalNumZeroes from process 0)
//   }

//   //Getting the amount of ones and zeroes on previous processors
//   int previousProcessorZeroes = 0;
//   int previousProcessorOnes = 0;
//   for (j in procNum) {
//     if (taskId == j) {
//       for (k = taskId to procNum) {
//         MPI_Send(localNumZeroes, k)
//         MPI_Send(localNumOnes)
//       }
//     }
//     else {
//       if (taskId > j) {
//         previousProcessorZeroes += MPI_Receive(localNumZeroes, j)
//         previousProcessorOnes += MPI_recieve(localNumOnes, j)
//       }
//     }
//   }

//   localSortedArray = empty.size(localArray)
//   for (j in localArray) {
//     int position
//     if (lcoalArray[j] == 0) {
//       position = j + previousProcessorZeroes
//     }
//     else {
//       position = j + previousProcessorOnes + totalProcessorZeroes
//     }
//     int destProcessor = position / numProc
//     int destOffset = position % numProc
//     MPI_Put(localArray[j] in localSortedArray[destOffset] in processor destProcessor)
//   }

//   MPI_Wait until all puts complete

//   localArray = localSortedArray
   
  
// }


// //Helper functions
// local_counting_sort(localArray, bitNumber) {

//   countingArray[2] = [0, 0] //Always 2 elements, number of 0s and 1s

//   for (i in size(localArray)) {
//     countingArray[(localArray[i] >> bitNumber) & 1]++
//   }
//   countingArray[1] += countArray[0]
//   numZeroes = countArray[0]

//   //Populate the output array
//   outputArray[size(localArray)]
//   for (i in size(outputArray)) {
//     outputArray[countArray[localArray[i]] - 1] = inputArray[i]
//     countArray[inputArray[i]]--
//   }
  
//   return outputArray, numZeroes
  
// }