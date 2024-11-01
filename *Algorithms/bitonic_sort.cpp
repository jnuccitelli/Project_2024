
/******************************************************************************
* FILE: bitonic_sort.cpp
* DESCRIPTION:  
*   Bitonic Sort Algorithm implementation
*   This code sorts arrays using bitonic sort!
* AUTHOR: Maximiliano Pombo
* LAST REVISED: 10/16/2024
******************************************************************************/
#include "mpi.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <caliper/cali.h> // for performance analysis
#include <caliper/cali-manager.h>
#include <adiak.hpp>
#include <queue>
#include <vector>

#define MASTER 0         

// Region definitions
const char* main_cali = "main";
const char* data_init_runtime = "data_init_runtime";
const char* correctness_check = "correctness_check";
const char* comm = "comm";
const char* comm_small = "comm_small";
const char* comm_large = "comm_large";
const char* comp = "comp";
const char* comp_small = "comp_small";
const char* comp_large = "comp_large";

// Function declarations
void bitonicSort(std::vector<int>& array, int size, int up);
void bitonicSeqMerge(std::vector<int>& array, int start, int BseqSize, int direction);
void bitonicSortrec(std::vector<int>& array, int start, int BseqSize, int direction);
void swap(int& first, int& second);
bool verifySorted(const std::vector<int>& array);
void mergeSortedArrays(std::vector<int>& array, int totalSize, int totalProcNum); // Updated function declaration

int main (int argc, char *argv[])
{
    CALI_CXX_MARK_FUNCTION;

    int sizeOfArray; 
    int inputType;
    MPI_Status status;

    cali::ConfigManager mgr;
    mgr.start();

    CALI_MARK_BEGIN(main_cali);
    CALI_MARK_BEGIN(data_init_runtime);

    // Input handling
    if (argc == 3) {
        sizeOfArray = atoi(argv[1]);
        inputType = atoi(argv[2]);
        if(inputType < 0 || inputType > 3) {
            printf("\n Please provide a valid input type [0-3]");
            return 0;
        }
    } else {
        printf("\n Please provide arguments [sizeOfArray] [input type]");
        return 0;
    }

    int totalProcNum;
    int taskId;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &totalProcNum);

    // Create an array of size sizeOfArray / proc count
    std::vector<int> localArray(sizeOfArray / totalProcNum);
    srand(std::time(0) + taskId); // Seed randomness based on process ID

    // Populate the array based on input type
    CALI_MARK_END(data_init_runtime);
    CALI_MARK_BEGIN(data_init_runtime);
    if (inputType == 0) { // Random input
        for (size_t i = 0; i < localArray.size(); ++i) {
            localArray[i] = rand() % 10000; // Random values
        }
    } else if (inputType == 1) { // Sorted input
        for (size_t i = 0; i < localArray.size(); ++i) {
            localArray[i] = i + (sizeOfArray * taskId);
        }
    } else if (inputType == 2) { // Reverse sorted input
        for (size_t i = 0; i < localArray.size(); ++i) {
            localArray[i] = sizeOfArray - i - 1 + (sizeOfArray * taskId);
        }
    } else if (inputType == 3) { // One percent permuted
        for (size_t i = 0; i < localArray.size(); ++i) {
            localArray[i] = i + (sizeOfArray * taskId);
        }
        // Randomly permute 1% of the array
        for (size_t i = 0; i < localArray.size() / 100; ++i) {
            int index = rand() % localArray.size();
            localArray[index] = rand() % 10000; // Random values
        }
    }
    CALI_MARK_END(data_init_runtime);

    printf("Initial array (Process %d): ", taskId);
    for (const auto& value : localArray) {
        printf("%d ", value);
    }
    printf("\n");

    // Perform Bitonic Sort on the local array
    CALI_MARK_BEGIN(comp);
    bitonicSort(localArray, localArray.size(), 1); // Sort in ascending order
    CALI_MARK_END(comp);

    // Gather sorted chunks back to the root process
    std::vector<int> sortedArray;
    if (taskId == MASTER) {
        sortedArray.resize(sizeOfArray);
    }

    CALI_MARK_BEGIN(comm);
    MPI_Gather(localArray.data(), localArray.size(), MPI_INT, sortedArray.data(), localArray.size(), MPI_INT, MASTER, MPI_COMM_WORLD);
    CALI_MARK_END(comm);

    // Perform final merge on the master process
    // if (taskId == MASTER) {
    //     mergeSortedArrays(sortedArray, sizeOfArray, totalProcNum); // Pass totalProcNum as an argument
    //     if (verifySorted(sortedArray)) {
    //         printf("Sorted array:\n");
    //         for (const auto& value : sortedArray) {
    //             printf("%d ", value);
    //         }
    //         printf("\n");
    //     } else {
    //         printf("The array is not sorted correctly.\n");
    //         for (const auto& value : sortedArray) {
    //             printf("%d ", value);
    //         }
    //         printf("\n");
    //     }
    // }
    if (taskId == MASTER) {
        mergeSortedArrays(sortedArray, sizeOfArray, totalProcNum);
        
        if (verifySorted(sortedArray)) {
            printf("Sorted array:\n");
            for (const auto& value : sortedArray) {
                printf("%d ", value);
            }
            printf("\n");
        } else {
            printf("The array is not sorted correctly.\n");
            for (const auto& value : sortedArray) {
                printf("%d ", value);
            }
            printf("\n");
        }
    }

    // Finalize Caliper
    adiak::init(NULL);
    adiak::launchdate();    
    adiak::libraries();     
    adiak::cmdline();       
    adiak::clustername();   
    adiak::value("algorithm", "bitonic"); 
    adiak::value("programming_model", "mpi"); 
    adiak::value("data_type", "int"); 
    adiak::value("size_of_data_type", sizeof(int)); 
    adiak::value("input_size", sizeOfArray); 
    adiak::value("input_type", (inputType == 0 ? "Random" :
                                inputType == 1 ? "Sorted" :
                                inputType == 2 ? "Reverse Sorted" : "1% Perturbed"));
    adiak::value("num_procs", totalProcNum); 
    adiak::value("scalability", "strong"); 
    adiak::value("group_num", 1); 
    adiak::value("implementation_source", "handwritten");

    mgr.stop();
    mgr.flush();

    MPI_Finalize();
    return 0;
}

// Bitonic Sort function
void bitonicSort(std::vector<int>& array, int size, int up) {
    bitonicSortrec(array, 0, size, up);
}

// Recursive function for bitonic sorting
void bitonicSortrec(std::vector<int>& array, int start, int BseqSize, int direction) {
    if (BseqSize > 1) {
        int k = BseqSize / 2;
        // Sort in ascending order if direction is 1, otherwise descending
        bitonicSortrec(array, start, k, 1);
        bitonicSortrec(array, start + k, k, 0);
        bitonicSeqMerge(array, start, BseqSize, direction);
    }
}

// Bitonic Sequence Merge function
void bitonicSeqMerge(std::vector<int>& array, int start, int BseqSize, int direction) {
    if (BseqSize > 1) {
        int k = BseqSize / 2;
        for (int i = start; i < start + k; i++) {
            if (direction == (array[i] > array[i + k])) {
                swap(array[i], array[i + k]);
            }
        }
        bitonicSeqMerge(array, start, k, direction);
        bitonicSeqMerge(array, start + k, k, direction);
    }
}

// Function to merge sorted arrays from all processes
// Function to merge sorted arrays from all processes
void mergeSortedArrays(std::vector<int>& array, int totalSize, int totalProcNum) {
    std::priority_queue<std::pair<int, int>> minHeap;
    
    // Push initial elements from each process into the heap
    for (int i = 0; i < totalSize; i++) {
        minHeap.push({array[i], i % totalProcNum}); // Use modulo to wrap around processes
    }
    
    // Merge sorted arrays
    for (int i = 0; i < totalSize; i++) {
        int smallestElement = minHeap.top().first;
        int processId = minHeap.top().second;
        minHeap.pop();
        
        array[i] = smallestElement;
        
        // Push next element from the same process into the heap
        int nextProcessId = (processId + 1) % totalProcNum;
        if (!minHeap.empty() && minHeap.top().second == nextProcessId) {
            minHeap.push({array[nextProcessId], nextProcessId});
        }
    }

    
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
