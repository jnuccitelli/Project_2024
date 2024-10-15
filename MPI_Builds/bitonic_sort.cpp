/******************************************************************************
* FILE: bitonic_sort.cpp
* DESCRIPTION:  
*   Bitonic Sort Algorithm implementation
*   Performs parallel bitonic sort on an array
* AUTHOR: Maximiliano Pombo
* LAST REVISED: 15/10/2024
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <caliper/cali.h> 
#include <caliper/cali-manager.h>
#include <adiak.hpp>

#define MASTER 0               
#define FROM_PARENT 1          
#define FROM_CHILD 2         
#define RANDOM_INPUT 0
#define SORTED_INPUT 1
#define REVERSE_SORTED_INPUT 2
#define ONE_PERC_PERMUTED 3

const char* main_cali = "main";
const char* data_init_runtime = "data_init_runtime";
const char* correctness_check = "correctness_check";
const char* comm = "comm";
const char* comm_small = "comm_small";
const char* comm_large = "comm_large";
const char* comp = "comp";
const char* comp_small = "comp_small";
const char* comp_large = "comp_large";


int* generateArray(int size, int inputType, int processorNumber, int totalProcesses);
void bitonicSort(int* array, int size, int currentProcessId, int totalProcesses);
void bitonicMerge(int* array, int size, int step, int sortDirection);
void swap(int &first, int &second);
bool verifySorted(int* arr, int size);
void printArray(int* arr, int size, int processorNumber);

int main (int argc, char *argv[]) {
    int sizeOfArray; // Size of the array
    int inputType;
    int taskid, numTasks;

    if (argc == 3) {
        sizeOfArray = atoi(argv[1]);
        inputType = atoi(argv[2]);
        if (inputType < 0 || inputType > 3) {
            printf("\n Please provide a valid input type [0-3]");
            return 0;
        }
    } else {
        printf("\n Please provide arguments [sizeOfArray] [input type]");
        return 0;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

    // Initialize Caliper for performance analysis
    cali::ConfigManager mgr;
    mgr.start();

    CALI_MARK_BEGIN(main_cali);
    CALI_MARK_BEGIN(data_init_runtime);
    int* arr = generateArray(sizeOfArray, inputType, taskid, numTasks);
    CALI_MARK_END(data_init_runtime);

    // Print starting arrays
    int rank = 0;
    while (rank < numTasks) {
        if (rank == taskid) {
            if (rank == 0) {
                printf("Starting Arrays\n");
            }
            printArray(arr, sizeOfArray, taskid);
            fflush(stdout);
        }
        rank++;
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Timing the sorting process
    CALI_MARK_BEGIN(main_cali);
    double start_time = MPI_Wtime();

    for (int stepSize = 2; stepSize <= sizeOfArray; stepSize *= 2) { 
        for (int subStep = stepSize / 2; subStep > 0; subStep /= 2) {
            // Determine the sort direction
            int sortDirection = (taskid % (stepSize / 2) == 0) ? 1 : 0;
            CALI_MARK_BEGIN(comp);
            CALI_MARK_BEGIN(comp_large);
            bitonicMerge(arr, sizeOfArray, subStep, sortDirection);
            CALI_MARK_END(comp_large);
            CALI_MARK_END(comp);
        }
    }

    double end_time = MPI_Wtime();
    CALI_MARK_END(main_cali);

    // Gather sorted chunks back to the root process
    int* sortedArray = nullptr;
    if (taskid == 0) {
        sortedArray = new int[sizeOfArray * numTasks];
    }
    MPI_Gather(arr, sizeOfArray, MPI_INT, sortedArray, sizeOfArray, MPI_INT, 0, MPI_COMM_WORLD);

    // Output the sorted array (only MASTER prints)
    if (taskid == 0) {
        printf("Total sorting time: %f seconds\n", end_time - start_time);
        // printArray(sortedArray, sizeOfArray * numTasks, taskid); // Uncomment to print the sorted array
    }

    // Check correctness
    CALI_MARK_BEGIN(correctness_check);
    if (!verifySorted(sortedArray, sizeOfArray * numTasks)) {
        printf("Array not sorted, processor %d\n", taskid);
    } else if (taskid == 0) {
        printf("Array Sorted!\n");
    }
    CALI_MARK_END(correctness_check);
    CALI_MARK_END(main_cali);

    // Cleanup and finalize MPI
    adiak::init(NULL);
    adiak::launchdate();    
    adiak::libraries();     
    adiak::cmdline();       
    adiak::clustername();   
    adiak::value("algorithm", "bitonic"); 
    adiak::value("programming_model", "mpi"); 
    adiak::value("data_type", "int"); 
    adiak::value("size_of_data_type", sizeof(int)); 
    adiak::value("input_size", sizeOfArray * numTasks); 
    adiak::value("input_type", (inputType == RANDOM_INPUT) ? "Random" : 
                              (inputType == SORTED_INPUT) ? "Sorted" : 
                              (inputType == REVERSE_SORTED_INPUT) ? "Reversed" : "Permuted");
    adiak::value("num_procs", numTasks); 
    adiak::value("scalability", "weak"); 
    adiak::value("group_num", 1); 
    adiak::value("implementation_source", "handwritten");

    mgr.stop();
    mgr.flush();

    MPI_Finalize();
    return 0;
}

// Function to generate input array based on input type
int* generateArray(int size, int inputType, int processorNumber, int totalProcesses) {
    int* arr = new int[size];

    if (inputType == RANDOM_INPUT) {
        for (int i = 0; i < size; i++) {
            arr[i] = rand() % 10000; 
        }
    } else if (inputType == SORTED_INPUT) {
        for (int i = 0; i < size; i++) {
            arr[i] = i + (size * processorNumber * 10);
        }
    } else if (inputType == REVERSE_SORTED_INPUT) {
        for (int i = size - 1; i >= 0; i--) {
            arr[i] = i + (size * processorNumber * 10);
        }
    } else { 
        for (int i = 0; i < size; i++) {
            arr[i] = i + (size * processorNumber * 10);
        }
        for (int i = 0; i < size; ++i) {
            if (rand() % 100 == 0) {
                arr[i] = rand();
            }
        }
    }
    return arr;
}

// Bitonic Sort function
void bitonicSort(int* array, int size, int currentProcessId, int totalProcesses) {
    for (int stepSize = 2; stepSize <= size; stepSize *= 2) { 
        for (int subStep = stepSize / 2; subStep > 0; subStep /= 2) {
            int sortDirection = (currentProcessId % (stepSize / 2) == 0) ? 1 : 0;
            bitonicMerge(array, size, subStep, sortDirection);
        }
    }
}

// Bitonic Merge function
void bitonicMerge(int* array, int size, int step, int sortDirection) {
    for (int i = 0; i < size; i++) {
        if ((sortDirection == 1 && array[i] > array[i + step]) || 
            (sortDirection == 0 && array[i] < array[i + step])) {
            swap(array[i], array[i + step]);
        }
    }
}

// Swap function
void swap(int &first, int &second) {
    int temp = first;
    first = second;
    second = temp;
}

// Function to verify if the array is sorted
bool verifySorted(int* arr, int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[i - 1]) {
            return false;
        }
    }
    return true;
}

// Function to print the array
void printArray(int* arr, int size, int processorNumber) {
    printf("Processor number %d, array: ", processorNumber);
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}


