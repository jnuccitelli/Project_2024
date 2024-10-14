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
    int* arr = new int[size];

    //random input
    if(inputType == RANDOM_INPUT) {
        for(int i = 0; i < size; i++) {
            arr[i] = rand() % 10000; //generates random ints
        }
        return arr;
    } 

    //sorted input
    else if(inputType == SORTED_INPUT) {
        int lastVal = 0;
        for(int i = 0; i < size; i++) {
            int newVal = ((rand() % 100) / 10) + lastVal;
            arr[i] = newVal; //generates ints from the last value to 10+last value
            lastVal = newVal;
            arr[i] += (size * processorNumber * 10); //offset for processor id
        }
        return arr;
    } 

    //reverse sorted input
    else if(inputType == REVERSE_SORTED_INPUT) {
        int lastVal = 0;
        for(int i = size-1; i >= 0; i--) {
            double newVal = ((rand() % 100) / 10) + lastVal;
            arr[i] = newVal; 
            lastVal = newVal;
            arr[i] += (size * (totalProcesses - processorNumber) * 10); //offset for processor id
        }
        return arr;
    } 

    //one percent permuted
    else { //one percent permuted
        //create sorted array
        int lastVal = 0;
        for(int i = 0; i < size; i++) {
            int newVal = ((rand() % 100) / 10) + lastVal;
            arr[i] = newVal; //generates ints from the last value to 10+last value
            lastVal = newVal;
            arr[i] += (size * processorNumber * 10); //offset for processor id
        }

        //randomly update 1% of elements
        for (int i =0; i < size; ++i) {
            if (rand() % 100 == 0) {
                arr[i] = rand();
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
    int numLocalZeroes;
    int numLocalOnes;
    int numTotalZeores;
    int numPrevZeroes;
    int numPrevOnes;
    int numPrevZeroesBuf;
    int numPrevOnesBuf;
    int position;
    int destProc;
    int destPos;

    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    MPI_Comm_size(MPI_COMM_WORLD,&numTasks);

    int* arr = generateArray(sizeOfArray, inputType, taskid, numTasks);

    //Print starting arrays
    int rank = 0;
    while (rank < numTasks) {
        if (rank == taskid) {
            if (rank == 0) {
                printf("Starting Arrays\n");
            }
            printArray(arr, sizeOfArray, taskid);
            fflush (stdout);
        }
        rank++;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    for (int i = 0; i < 1; ++i) {


        //performing a local counting sort
        int countingArray[2] = {0, 0};

        for (int j = 0; j < sizeOfArray; ++j) {
            countingArray[(arr[j] >> i) & 1]++;
        }
        countingArray[1] += countingArray[0];
        numLocalZeroes = countingArray[0];

        int* newArr = new int[sizeOfArray];

        for (int j = sizeOfArray - 1; j >= 0; --j) {
            newArr[countingArray[(arr[j] >> i) & 1] - 1] = arr[j];
            countingArray[(arr[j] >> i) & 1]--;
        }

        delete[] arr;
        arr = newArr;

        printf("process: %d copied elements\n", taskid);
        

        //Get total number of 0s to all processes
        numLocalOnes = sizeOfArray - numLocalZeroes;
        printf("process: %d, numZeroes: %d, numOnes: %d\n", taskid, numLocalZeroes, numLocalOnes);

        
        for (int j = 0; j < numTasks; j++) {
            MPI_Reduce(&numLocalZeroes, &numTotalZeores, 1, MPI_INT, MPI_SUM, j, MPI_COMM_WORLD);
        }
        printf("process: %d, totalNumZeroes: %d\n", taskid, numTotalZeores);

        //Get the previous numbers of 0s and 1s to determine placement
        numPrevZeroes = 0;
        numPrevOnes = 0;
        for (int j = 0; j < numTasks; ++j) {
            if (j == taskid) {
                for (int k = taskid + 1; k < numTasks; ++k) {
                    MPI_Send(&numLocalZeroes, 1, MPI_INT, k, 1, MPI_COMM_WORLD);
                    MPI_Send(&numLocalOnes, 1, MPI_INT, k, 1, MPI_COMM_WORLD);
                }
            }
            else {
                if (j < taskid) {
                    MPI_Recv(&numPrevZeroesBuf, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &status);
                    MPI_Recv(&numPrevOnesBuf, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &status);
                    numPrevZeroes += numPrevZeroesBuf;
                    numPrevOnes += numPrevOnesBuf;
                }
            }
        }
        printf("process: %d, prevNumZeroes: %d, prevNumOnes: %d\n", taskid, numPrevZeroes, numPrevOnes);

        //Move elements to their sorted arrays
        int* newSortedArr = new int[sizeOfArray + 1];
        int destProcArray[sizeOfArray];
        int destPosArray[sizeOfArray];
        MPI_Win win;
        MPI_Win_create(&newSortedArr, ((MPI_Aint)sizeOfArray + 1) * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

        for (int j = 0; j < sizeOfArray; ++j) {
            if ((int) ((arr[j] >> i) & 1) == 0) {
                position = j + numPrevZeroes;
                //printf("position: %d, j: %d, numPrevZeroes: %d\n", position, j, numPrevZeroes);
            }
            else {
                position = j + numTotalZeores + numPrevOnes - numLocalZeroes;
            }
            destProcArray[j] = position / sizeOfArray;
            destPosArray[j] = position % sizeOfArray;
        }

            
        MPI_Barrier(MPI_COMM_WORLD);
        for (int l = 0; l < 1; ++l) {
            MPI_Win_fence(0, win);
            MPI_Barrier(MPI_COMM_WORLD);
            printf("Process %d, l %d\n", taskid, l);
            if (l == taskid) {
                printf("Starting process %d copy\n", l);
                for (int j = 0; j < sizeOfArray; ++j) {
                    if (destProcArray[j] != taskid) {
                        printf("process: %d, element: %d, position: %d, destProc: %d, destPos: %d\n", taskid, arr[j], destProcArray[j] * sizeOfArray + destPosArray[j], destProcArray[j], destPosArray[j]);
                        MPI_Put(&(arr[j]), 1, MPI_INT, destProcArray[j], (MPI_Aint)destPosArray[j], 1, MPI_INT, win);
                    }
                }
                fflush (stdout);
            }
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Win_fence(0, win);
        }
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Win_free(&win);
        delete[] arr;
        arr = newSortedArr;
        
    }


    //Print finished arrays
    rank = 0;
    while (rank < numTasks) {
        if (rank == taskid) {
            if (rank == 0) {
                printf("Finished Arrays\n");
            }
            printArray(arr, sizeOfArray, taskid);
            fflush (stdout);
        }
        rank++;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
}





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