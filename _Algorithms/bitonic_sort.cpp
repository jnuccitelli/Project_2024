/******************************************************************************
* FILE: bitonic_sort.cpp
* DESCRIPTION:  
*   Bitonic Sort Algorithm implementation with Caliper instrumentation.
* AUTHOR: Maximiliano Pombo
* LAST REVISED: 10/30/2024
******************************************************************************/

#include "mpi.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <caliper/cali.h> 
#include <caliper/cali-manager.h>
#include <adiak.hpp>
#include <queue>

#define MASTER 0


void bitonicSort(std::vector<int>& array, int size, int up);
void bitonicSeqMerge(std::vector<int>& array, int start, int BseqSize, int direction);
void bitonicSortrec(std::vector<int>& array, int start, int BseqSize, int direction);
void swap(int& first, int& second);
bool verifySorted(const std::vector<int>& array);
void mergeSortedArrays(std::vector<int>& array, int totalSize, int totalProcNum);

int main(int argc, char *argv[])
{
    CALI_MARK_BEGIN("main");

    int sizeOfArray, inputType;
    MPI_Status status;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    int totalProcNum, taskId;
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcNum);

    cali::ConfigManager mgr;
    mgr.start();

    // Input handling
    if (argc == 3) {
        sizeOfArray = atoi(argv[1]);
        inputType = atoi(argv[2]);
        if (inputType < 0 || inputType > 3) {
            if (taskId == MASTER) {
                printf("Please provide a valid input type [0-3]\n");
            }
            MPI_Finalize();
            return 0;
        }
    } else {
        if (taskId == MASTER) {
            printf("Please provide arguments [sizeOfArray] [input type]\n");
        }
        MPI_Finalize();
        return 0;
    }


    int localArraySize = sizeOfArray / totalProcNum;
    std::vector<int> localArray(localArraySize);
    srand(std::time(0) + taskId);

    // Populate local array based on input type
    CALI_MARK_BEGIN("data_init_runtime");
    if (inputType == 0) {
        for (int& val : localArray) {
            val = rand() % 10000;
        }
    } else if (inputType == 1) {
        for (int i = 0; i < localArraySize; ++i) {
            localArray[i] = i + (sizeOfArray * taskId);
        }
    } else if (inputType == 2) {
        for (int i = 0; i < localArraySize; ++i) {
            localArray[i] = sizeOfArray - i - 1 + (sizeOfArray * taskId);
        }
    } else if (inputType == 3) {
        for (int i = 0; i < localArraySize; ++i) {
            localArray[i] = i + (sizeOfArray * taskId);
        }
        for (int i = 0; i < localArraySize / 100; ++i) {
            int index = rand() % localArraySize;
            localArray[index] = rand() % 10000;
        }
    }
    CALI_MARK_END("data_init_runtime");

    // Display initial local array
    printf("Initial array (Process %d): ", taskId);
    for (const int& val : localArray) {
        printf("%d ", val);
    }
    printf("\n");

    // Perform Bitonic Sort on local array
    CALI_MARK_BEGIN("comp");
    CALI_MARK_BEGIN("comp_large");
    bitonicSort(localArray, localArray.size(), 1);
    CALI_MARK_END("comp_large");
    CALI_MARK_END("comp");

    // Gather sorted chunks at root process
    std::vector<int> sortedArray;
    if (taskId == MASTER) {
        sortedArray.resize(sizeOfArray);
    }

    CALI_MARK_BEGIN("comm");
    CALI_MARK_BEGIN("comm_large");
    CALI_MARK_BEGIN("MPI_Gather");
    MPI_Gather(localArray.data(), localArray.size(), MPI_INT, 
               sortedArray.data(), localArray.size(), MPI_INT, 
               MASTER, MPI_COMM_WORLD);
    CALI_MARK_END("MPI_Gather");
    CALI_MARK_END("comm_large");
    CALI_MARK_END("comm");


    CALI_MARK_BEGIN("comm");
    CALI_MARK_BEGIN("comm_small");
    CALI_MARK_END("comm_small");
    CALI_MARK_END("comm");


    if (taskId == MASTER) {
        CALI_MARK_BEGIN("comp");
        CALI_MARK_BEGIN("comp_small");
        mergeSortedArrays(sortedArray, sizeOfArray, totalProcNum);
        CALI_MARK_END("comp_small");
        CALI_MARK_END("comp");

        CALI_MARK_BEGIN("correctness_check");
        if (verifySorted(sortedArray)) {
            printf("Array sorted correctly!\n");
        } else {
            printf("Array sorting failed!\n");
        }
        CALI_MARK_END("correctness_check");
    }


    mgr.stop();
    mgr.flush();
    CALI_MARK_END("main");
    MPI_Finalize();

    return 0;
}


void bitonicSort(std::vector<int>& array, int size, int up) {
    CALI_MARK_BEGIN("comp_small");
    bitonicSortrec(array, 0, size, up);
    CALI_MARK_END("comp_small");
}


void bitonicSortrec(std::vector<int>& array, int start, int BseqSize, int direction) {
    if (BseqSize > 1) {
        int k = BseqSize / 2;
        bitonicSortrec(array, start, k, 1);
        bitonicSortrec(array, start + k, k, 0);
        bitonicSeqMerge(array, start, BseqSize, direction);
    }
}


void bitonicSeqMerge(std::vector<int>& array, int start, int BseqSize, int direction) {
    if (BseqSize > 1) {
        int k = BseqSize / 2;
        for (int i = start; i < start + k; ++i) {
            if (direction == (array[i] > array[i + k])) {
                swap(array[i], array[i + k]);
            }
        }
        bitonicSeqMerge(array, start, k, direction);
        bitonicSeqMerge(array, start + k, k, direction);
    }
}


void mergeSortedArrays(std::vector<int>& array, int totalSize, int totalProcNum) {
    CALI_MARK_BEGIN("comp_small");
    std::priority_queue<std::pair<int, int>> minHeap;
    for (int i = 0; i < totalSize; i++) {
        minHeap.push({array[i], i % totalProcNum});
    }
    for (int i = 0; i < totalSize; i++) {
        array[i] = minHeap.top().first;
        minHeap.pop();
    }
    CALI_MARK_END("comp_small");
}

// Swap helper function
void swap(int& first, int& second) {
    int temp = first;
    first = second;
    second = temp;
}

// Function to verify if the array is sorted
bool verifySorted(const std::vector<int>& array) {
    for (size_t i = 1; i < array.size(); ++i) {
        if (array[i] < array[i - 1]) {
            return false;
        }
    }
    return true;
}
