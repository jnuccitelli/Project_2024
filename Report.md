# CSCE 435 Group project

## 0. Group number: 
Group 1, Section 200

## 1. Group members:
1. Ariela Mitrani
2. Joseph Nuccitelli
3. Alexander Nuccitelli
4. Patralika Ghosh
5. Maximiliano Pombo

## 2. Parallel sorting algorithms

### 2a. Project communication
For this project, our team will communicate via Discord for planning, organization, and collaboration.

### 2b. Brief project description (what algorithms will you be comparing and on what architectures)
For this project, we will be comparing the following 5 algorithms:
- Bitonic Sort (Maximiliano)
- Sample Sort (Joseph)
- Merge Sort (Ariela)
- Radix Sort (Alex)
- Column Sort (Patralika)
  
We will be implementing everything with MPI for this project.

### 2c. Pseudocode for each parallel algorithm
- For MPI programs, include MPI calls you will use to coordinate between processes
#### Bitonic Sort: Maximiliano
```
// Bitonic Sort is a parallel sorting algorithm that is efficient for hardware implementations and works well in a parallel computing environment.
// It requires a bitonic Sequence which is a sequence that first increases and then decreases, or is entirely increasing or decreasing.
/* Fist thing to do is recursively divide the sequence into two halves, creating a bitonic sequence from the two halves.
Then merge the bitonic sequences into larger bitonic sequences until the entire sequence is sorted.
*/
//Time Complexity of O(log2n)

int totalElements = user_input_for_array_size;
int totalProcesses;
int currentProcessId;

MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &currentProcessId);
MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);

int chunkSize = totalElements / totalProcesses;
int[] localChunk = new int[chunkSize];

// Generate or receive the local part of the array
if (currentProcessId == 0) {
    int[] completeArray = generateArray(totalElements);
    MPI_Scatter(completeArray, chunkSize, MPI_INT, localChunk, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
} else {
    MPI_Scatter(NULL, chunkSize, MPI_INT, localChunk, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
}

// Bitonic Sort on the local chunk
bitonicSort(localChunk, chunkSize, currentProcessId, totalProcesses);

// Gather the sorted chunks back to the root process
int[] sortedArray;
if (currentProcessId == 0) {
    sortedArray = new int[totalElements];
}
MPI_Gather(localChunk, chunkSize, MPI_INT, sortedArray, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

// Output the sorted array
if (currentProcessId == 0) {
    printf(sortedArray); 
}
return;

// Perform Bitonic Sort
void bitonicSort(int[] array, int size, int currentProcessId, int totalProcesses) {
    for (int stepSize = 2; stepSize <= size; stepSize *= 2) { 
        for (int subStep = stepSize / 2; subStep > 0; subStep /= 2) {
            // Determine the direction (ascending or descending)
            int sortDirection = (currentProcessId % (stepSize / 2) == 0) ? 1 : 0;
            bitonicMerge(array, size, subStep, sortDirection);
        }
    }
}

// Perform Bitonic Merge
void bitonicMerge(int[] array, int size, int step, int sortDirection) {
    for (int i = 0; i < size; i++) {
        // Compare and swap based on direction
        if ((sortDirection == 1 && array[i] > array[i + step]) || (sortDirection == 0 && array[i] < array[i + step])) {
            swap(array[i], array[i + step]);
        }
    }

    // Send and/or receive data from neighbors
    if (totalProcesses > 1) {
        MPI_Sendrecv(array, size, MPI_INT, neighborProcessId, 0, 
                     array, size, MPI_INT, neighborProcessId, 0, MPI_COMM_WORLD, &status);
    }
}

// Swap elements helper function 
void swap(int &first, int &second) {
    int temp = first;
    first = second;
    second = temp;
}

```
#### Sample Sort: Joseph
```
int arraySize = user input for array size;
int procNum;
int taskId;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&procNum);

if(procNum == 0){
  //create the array and fill it with elements
}
MPI_Scatter(array, sizeof(array)/nbuckets, MPI_DOUBLE,procArray, sizeof(array)/nbuckets, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//quick sort the samples
std::sort(procArray);
MPI_Gather(all quick sorted elements);
//select the splitters using quicksort each process will do this to avoid extra communication

//put each element in the bucket
for(i = 0; i < size/nbuckets; ++i){
  correctBucket.insert(procArray[i]);
}
//each process sorts each bucket using quick sort
std::sort(bucket);

//array is sorted now


```
#### Merge Sort: Ariela
```
int arraySize = user input for array size;
int procNum;
int taskId;

MPI_Init(&argc,&argv);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm_size(MPI_COMM_WORLD,&procNum);

for(n in procNum) {
  if(taskid == n) {
    int children = childCount(n, procNum); //check if left child 2n+1 is in range, right child 2n+2 is in range

    int[] array;
    if(taskid == 0)
      array = generateArray(arraySize); //this fills our array with values
      startChildProcesses(n, children, array, arraySize)
    else
      MPI_recv(array from parent process)
      MPI_send(startChildProcesses(n, children, array, arraySize) to our parent task ((n-2)/2 if even, (n-1)/2 if odd))
  }
}

if(taskid == 0)
  print(finalArray)

return; //end of program main

//helper function 1
array startChildProcesses(myId, numChildren, array, arraySize) {
  if(numChildren == 0)
    return mergeSort(array, arraySize)
  if(numChildren == 1)
    MPI_send(left half of array and size to 2*myId+1)
    sortedRight = mergeSort(right half of array, arraySize/2)
    sortedLeft; //empty array
    MPI_recv(sortedLeft from 2*myId+1)
    return combineSortedArrays(sortedRight, sortedLeft)
  if(numChildren == 2)
    MPI_send(left half of array and size to 2*myId+1)
    MPI_send(right half of array and size to 2*myId+2)
    sortedLeft; //empty array
    sortedRight; //empty array
    MPI_recv(sortedLeft from 2*myId+1)
    MPI_recv(sortedRight from 2*myId+2)
    return combineSortedArrays(sortedRight, sortedLeft, arraySizeRight, arraySizeLeft)
}

//helper function 2
mergeSort(array, arraySize) { //sequential implementation for leaf nodes
  if(arraySize = 1)
    return array;
  else
    mergesort(arrayRight, arraySizeRight)
    mergesort(arrayLeft, arraySizeLeft) //this is an estimate
    return combineSortedArrays(arrayRight, arrayLeft, arraySizeRight, arraySizeLeft)
}

//helper function 3
combineSortedArrays(sortedRight, sortedLeft, arraySizeRight, arraySizeLeft) {
  returnArray[arraySizeRight+arraySizeLeft]
  int r = 0;
  int l = 0;
  for(int i in arraySizeRight+arraySizeLeft){
    if(r = arraySizeRight) {
      returnArray[i] = sortedLeft[l]
      l++
    }
    else if(l = arraySizeLeft) {
      returnArray[i] = sortedRight[r]
      r++
    }
    else if(sortedRight[r] < sortedLeft[l]) {
      returnArray[i] = sortedRight[r]
      r++
    }
    else {
      returnArray[i] = sortedLeft[l]
      l++
    }
  }
}
```

#### Radix Sort: Alex
```
int arraySize = user input for array size
int procNum
int taskId

MPI_Init(&argc,&argv)
MPI_Comm_rank(MPI_COMM_WORLD,&taskid)
MPI_Comm_size(MPI_COMM_WORLD,&procNum)

int totalArray[arraySize]

//Generate the array
for (i in procNum) {
  int offset = arraySize / ProcNum * taskId
  totalArray[from offset to (offset + (arraySize/ProcNum)] = Array generation
}

//sort the array
for (i in numBits of type) {
  int[arraySize / ProcNum] localArray = totalArray[from offset to (offset + (arraySize/ProcNum)]

  //get total zeroes to all processes
  int localNumZeroes
  localArray, localNumZeroes = local_counting_sort(localArray, bitnumber = i)
  int localNumOnes = size(localArray) - localNumZeroes
  int totalNumZeroes
  MPI_Reduce(reduce local_num_zeroes to total_num_zeroes to process 0)
  if (taskId == 0) {
    MPI_Send(totalNumZeroes)
  }
  else {
    MPI_Recieve(totalNumZeroes from process 0)
  }

  //Getting the amount of ones and zeroes on previous processors
  int previousProcessorZeroes = 0;
  int previousProcessorOnes = 0;
  for (j in procNum) {
    if (taskId == j) {
      for (k = taskId to procNum) {
        MPI_Send(localNumZeroes, k)
        MPI_Send(localNumOnes)
      }
    }
    else {
      if (taskId > j) {
        previousProcessorZeroes += MPI_Receive(localNumZeroes, j)
        previousProcessorOnes += MPI_recieve(localNumOnes, j)
      }
    }
  }

  localSortedArray = empty.size(localArray)
  for (j in localArray) {
    int position
    if (lcoalArray[j] == 0) {
      position = j + previousProcessorZeroes
    }
    else {
      position = j + previousProcessorOnes + totalProcessorZeroes
    }
    int destProcessor = position / numProc
    int destOffset = position % numProc
    MPI_Put(localArray[j] in localSortedArray[destOffset] in processor destProcessor)
  }

  MPI_Wait until all puts complete

  localArray = localSortedArray
   
  
}


//Helper functions
local_counting_sort(localArray, bitNumber) {

  countingArray[2] = [0, 0] //Always 2 elements, number of 0s and 1s

  for (i in size(localArray)) {
    countingArray[(localArray[i] >> bitNumber) & 1]++
  }
  countingArray[1] += countArray[0]
  numZeroes = countArray[0]

  //Populate the output array
  outputArray[size(localArray)]
  for (i in size(outputArray)) {
    outputArray[countArray[localArray[i]] - 1] = inputArray[i]
    countArray[inputArray[i]]--
  }
  
  return outputArray, numZeroes
  
}
```
#### Column Sort: Patralika
```
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ARRAY_SIZE 16 // Define array size
#define DIMENSION 4   // Dimension for 2D grid (sqrt(ARRAY_SIZE))

// Function to sequentially sort an array (use any sorting algorithm here)
void sequentialSort(int *array, int size) {
    // Simple bubble sort for demonstration
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

// Main function
int main(int argc, char** argv) {
    int taskId, procNum;
    int arraySize = ARRAY_SIZE;
    int arrayDimension = DIMENSION;
    
    int globalArray[arraySize];    // Global array (only needed by the root process)
    int localData[arrayDimension]; // Array for local process
    int shuffledData[arrayDimension]; // Temporary array for shuffling

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);

    // Root process initializes the global array with some values
    if (taskId == 0) {
        for (int i = 0; i < arraySize; i++) {
            globalArray[i] = arraySize - i;  // Just an example: [16, 15, ..., 1]
        }
    }

    // Step 1: Scatter the 2D array (divided into columns) to all processes
    MPI_Scatter(globalArray, arrayDimension, MPI_INT, localData, arrayDimension, MPI_INT, 0, MPI_COMM_WORLD);

    // Step 2: Each process sorts its assigned column
    sequentialSort(localData, arrayDimension);

    // Step 3: All processes permute rows (shuffle rows among processes)
    MPI_Alltoall(localData, arrayDimension, MPI_INT, shuffledData, arrayDimension, MPI_INT, MPI_COMM_WORLD);

    // Step 4: Each process sorts the shuffled rows
    sequentialSort(shuffledData, arrayDimension);

    // Step 5: Reverse the row permutation (transpose back)
    MPI_Alltoall(shuffledData, arrayDimension, MPI_INT, localData, arrayDimension, MPI_INT, MPI_COMM_WORLD);

    // Step 6: Final column sort (each process sorts its final column)
    sequentialSort(localData, arrayDimension);

    // Gather the final sorted columns into the global array (in the root process)
    MPI_Gather(localData, arrayDimension, MPI_INT, globalArray, arrayDimension, MPI_INT, 0, MPI_COMM_WORLD);

    // Root process prints the sorted global array
    if (taskId == 0) {
        printf("Sorted Array:\n");
        for (int i = 0; i < arraySize; i++) {
            printf("%d ", globalArray[i]);
        }
        printf("\n");
    }

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}


```

### 2d. Evaluation plan - what and how will you measure and compare
- Compare different algorithms to sort and see how the performance differs across them
- Compare communication strategies; Collective vs point-to-point
- Compare different parallelization strategies (master/worker vs SPMD)
- Compare different input sizes with both strong and weak scaling (so increasing the input size with and without increasing the number of processes)
- Compare different input types; Randomly generated, already sorted, sorted in reverse, sorted with 1% permuted
