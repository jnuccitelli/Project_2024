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

<details>
  <summary><i>Bitonic Sort</i></summary>
  
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

</details>

<details>
  <summary><i>Sample Sort</i></summary>
  
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
</details>

<details>
  <summary><i>Merge Sort</i></summary>
  
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

</details>

<details>
  <summary><i>Radix Sort</i></summary>

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

</details>

<details>
  <summary><i>Column Sort</i></summary> 
  
#### Column Sort: Patralika
```
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ARRAY_SIZE 16 
#define DIMENSION 4   // Dimension for 2D grid (sqrt(ARRAY_SIZE))


void sequentialSort(int *array, int size) {
   
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

// Helper function for debugging
void printArray(const char* title, int* array, int size) {
    printf("%s:\n", title);
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}


int main(int argc, char** argv) {
    int taskId, procNum;
    int arraySize = ARRAY_SIZE;
    int arrayDimension = DIMENSION;
    
    int globalArray[arraySize];    // Global array for the root process
    int localData[arrayDimension]; // Array for local process
    int shuffledData[arrayDimension]; // Temporary array for shuffling

    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Comm_size(MPI_COMM_WORLD, &procNum);

    
    if (taskId == 0) {
        for (int i = 0; i < arraySize; i++) {
            globalArray[i] = arraySize - i;  
        }
    }

    // Step 1: Scatter the 2D array (divided into columns) to all processes
    MPI_Scatter(globalArray, arrayDimension, MPI_INT, localData, arrayDimension, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Print local data for each process after scattering (debugging)
    // printf("Process %d received data: ", taskId);
    // for (int i = 0; i < arrayDimension; i++) {
    //     printf("%d ", localData[i]);
    // }
    // printf("\n");

    // Step 2: Each process sorts its assigned column
    sequentialSort(localData, arrayDimension);

    // Step 3: Perform row-wise transposition using MPI_Alltoall
    MPI_Alltoall(localData, 1, MPI_INT, shuffledData, 1, MPI_INT, MPI_COMM_WORLD);

    // Print shuffled data for each process after all-to-all (debugging)
    // printf("Process %d after Alltoall: ", taskId);
    // for (int i = 0; i < arrayDimension; i++) {
    //     printf("%d ", shuffledData[i]);
    // }
    // printf("\n");

    // Step 4: Each process sorts the shuffled rows
    sequentialSort(shuffledData, arrayDimension);

    // Step 5: Reverse the row transposition (transpose back)
    MPI_Alltoall(shuffledData, 1, MPI_INT, localData, 1, MPI_INT, MPI_COMM_WORLD);

    // Step 6: Final column sort (each process sorts its final column)
    sequentialSort(localData, arrayDimension);

    // Gather the final sorted columns into the global array (in the root process)
    MPI_Gather(localData, arrayDimension, MPI_INT, globalArray, arrayDimension, MPI_INT, 0, MPI_COMM_WORLD);

    // Root process prints the sorted global array
    if (taskId == 0) {
        printArray("Sorted Array", globalArray, arraySize);
    }

    
    MPI_Finalize();

    return 0;
}

```
</details>

### 2d. Evaluation plan - what and how will you measure and compare
- Compare different algorithms to sort and see how the performance differs across them
- Compare communication strategies; Collective vs point-to-point
- Compare different parallelization strategies (master/worker vs SPMD)
- Compare different input sizes with both strong and weak scaling (so increasing the input size with and without increasing the number of processes)
- Compare different input types; Randomly generated, already sorted, sorted in reverse, sorted with 1% permuted

## 3. Descriptions of Implemented Algorithms
In this section, we will discuss the ways we implemented our various sorting algorithms, such as how they communicate, how the computation workload is split, etc.

<details>
  <summary><i>Merge Sort</i></summary>
  
### Merge Sort - Ariela
My merge sort implementation was similar to my pseudocode, but after running analysis on the algorithm, I realized that I wasn't using the processors as efficiently as I could have. To fix this, I distributed the work evenly over all of the nodes by having them each generate their own data and sort it, then used the original parent-child hierarchy to determine which nodes should be executing the merge. Essentially, every node starts with an array of length totalArraySize/numNodes, sorts that, and then sends the sorted array to a specific task that will be responsible for merging the arrays together. Thus, in every step there are half the amount of processes working as there were in the previous step, which is the maximum amount of processes that can be working at one time for merge sort. 

Thicket tree:
![merge_thicket_tree](ThicketTrees/mergethicket.png)

</details>

<details>
  <summary><i>Radix Sort</i></summary>
  
### Radix Sort - Alex
This implmentation of radix sort only works for integers. It first generates arrays in each process. Each process has one segement of the array. After that it performs a local counting sort on the least significant bit of each integer. It also gets the local number of 0 bits, which come before the 1 bits. After sorting locally, the local number of 0's and 1's is distributed accordingly such that each processor has the total number of 0 bits, as well as the amount of 0 and 1 bits present in previous processors. This allows each processor to get the location of the sorted element. This has to be a stable sort, since otherwise the order would be messed up. Finally each element is moved to it's new position in reation to the total array. Intially in the psuedocode the implmentation used 1 sided communication using MPI_Put, however even though this would improve performance, it kept writing values to incorrect spots in memory. The actual implmenation uses 2 sided communication. The correctness checking first checks each array, and then checks the last element of the previous array with the first element of the next one.

Call Tree:
![image](https://github.com/user-attachments/assets/271a69e5-be2c-47c4-9b50-17839adb1e19)
Meta Data:
![image](https://github.com/user-attachments/assets/eaaafc7e-c83b-456a-8bac-c42159de99d5)
</details>

<details>
  <summary><i>Bitonic Sort</i></summary>
  
### Bitonic Sort - Maximiliano 
My implementation of the bitonic sort algorithm closely followed the pseudocode, but I made several optimizations to ensure efficient use of the available processors. Each processor begins by generating its own segment of the array, with the size of each segment determined by the total array size divided by the number of processors. This approach allows each node to perform a local bitonic sort on its chunk of the array independently.
The bitonic sort operates in multiple stages. In each stage, the processors first determine the sorting direction based on their rank and the current step size. This hierarchical structure allows for efficient merging of the sorted segments, where pairs of processors collaborate to perform the bitonic merge operation. By dynamically adjusting the step size and merging the results, I ensured that the number of active processors reduces by half in each subsequent step, maximizing parallelism during the sorting process.

Call Tree:
![column_thicket_tree](/ThicketTrees/BitonicTree.png)
Meta Data:
![column_metadata](/Metadata/BitonicMetadata.png)
 </details>

<details>
  <summary><i>Column Sort</i></summary>
  
### Column Sort - Patralika Ghosh
My implementation of the column sort closely follows the structure of my pseudocode and is largely based on Leighton's Column Sort algorithm. The algorithm generates the array according to the input type, and each process is assigned a portion of indexes based on the input size and the number of processes. Each process then sorts its assigned column and returns the sorted result. Next, I transpose the matrix with the sorted columns and reshape it into submatrices of size (r/c) X r, where r is the number of rows and c is the number of columns.
I repeat the process of column sorting, transposing, and reshaping two more times. Afterward, I perform a shift in the matrix by adding an extra column, where one half is filled with -inf values and the other half with +inf values. I sort the columns again, then remove the -inf and +inf values, resulting in a fully column-sorted matrix.


![column_thicket_tree](/ThicketTrees/columnthicket.png)
![column_metadata](/Metadata/columnMetadata.png)

</details>

<details>
  <summary><i>Sample Sort</i></summary>

### Sample Sort - Joseph Nuccitelli
First each process in my code generates their local splitters. They do this by sorting their local data and sampling splitters. Then each process sends their local splitters to the main process. The main process then sorts all of the local samples and creates global splitters. These are then broadcasted out to everyone. Then each process puts each element into the array into a bucket. The last part each process is given a bucket. They copy their given bucket and then send other processes buckets to each process. After that each process sorts their new bucket and we have a sorted array.

![image](https://github.com/user-attachments/assets/568b0958-94ce-4325-8a2c-3c12047e5180)

</details>

## 4. Performance evaluation
<details>
  <summary><i>Sample Sort</i></summary>

![comm_65536](/Graphs/GraphsSampleSort/comm_65536.png)
![comm_262144](/Graphs/GraphsSampleSort/comm_262144.png)
![comm_1048576](/Graphs/GraphsSampleSort/comm_1048576.png)
![comm_4194304](/Graphs/GraphsSampleSort/comm_4194304.png)
![comm_16777216](/Graphs/GraphsSampleSort/comm_16777216.png)
![comm_67108864](/Graphs/GraphsSampleSort/comm_67108864.png)  
Comm time vs threads. This followed the basic trend of increasing communication as process time increased. There are some blips in the graph that can be explained when there is a high amount of buckets compared to array size. What surprised me the most was how for big array sizes communication decreased at first. This on further thought makes sense as one of the factors of the complex bucket size vs. array size relationship. 
![comp_65536](/Graphs/GraphsSampleSort/comp_65536.png)
![comp_262144](/Graphs/GraphsSampleSort/comp_262144.png)
![comp_1048576](/Graphs/GraphsSampleSort/comp_1048576.png)
![comp_4194304](/Graphs/GraphsSampleSort/comp_4194304.png)
![comp_16777216](/Graphs/GraphsSampleSort/comp_16777216.png)
![comp_67108864](/Graphs/GraphsSampleSort/comp_67108864.png)
Comp time vs threads for various array sizes. These graphs are as expected. As the amount of threads increases so does the amount of buckets meaning that bucket size decreases. This means that each thread will have to sort less elements meaning that there will be less computation time. 
![main_65536](/Graphs/GraphsSampleSort/main_65536.png)
![main_262144](/Graphs/GraphsSampleSort/main_262144.png)
![main_1048576](/Graphs/GraphsSampleSort/main_1048576.png)
![main_4194304](/Graphs/GraphsSampleSort/main_4194304.png)
![main_16777216](/Graphs/GraphsSampleSort/main_16777216.png)
![main_67108864](/Graphs/GraphsSampleSort/main_67108864.png)
Main time vs process count for various array sizes. At large array sizes my program behaves as intended. With large array sizes total time is decreased as expected. With small array sizes though this is not always the case. Sorting is scaled on N log N while communication is scaled linearly. However at low array sizes communication takes the most time out of all the algorithms. Blips can be seen in some of the graphs when the proc count goes to 64 this is a result of grace being separated between different nodes. These trends make a lot of sense given the graphs.
![main_permuted_strong_speedup](/Graphs/GraphsSampleSort/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](/Graphs/GraphsSampleSort/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](/Graphs/GraphsSampleSort/main_random_strong_speedup.png)
![main_random_weak_efficiency](/Graphs/GraphsSampleSort/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/Graphs/GraphsSampleSort/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/Graphs/GraphsSampleSort/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/Graphs/GraphsSampleSort/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/Graphs/GraphsSampleSort/main_sorted_weak_efficiency.png)
![comm_permuted_strong_speedup](/Graphs/GraphsSampleSort/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/Graphs/GraphsSampleSort/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/Graphs/GraphsSampleSort/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/Graphs/GraphsSampleSort/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/Graphs/GraphsSampleSort/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/Graphs/GraphsSampleSort/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/Graphs/GraphsSampleSort/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/Graphs/GraphsSampleSort/comm_sorted_weak_efficiency.png)

![comp_permuted_strong_speedup](/Graphs/GraphsSampleSort/comp_permuted_strong_speedup.png)
![comp_permuted_weak_efficiency](/Graphs/GraphsSampleSort/comp_permuted_weak_efficiency.png)
![comp_random_strong_speedup](/Graphs/GraphsSampleSort/comp_random_strong_speedup.png)
![comp_random_weak_efficiency](/Graphs/GraphsSampleSort/comp_random_weak_efficiency.png)
![comp_reversed_strong_speedup](/Graphs/GraphsSampleSort/comp_reversed_strong_speedup.png)
![comp_reversed_weak_efficiency](/Graphs/GraphsSampleSort/comp_reversed_weak_efficiency.png)
![comp_sorted_strong_speedup](/Graphs/GraphsSampleSort/comp_sorted_strong_speedup.png)
![comp_sorted_weak_efficiency](/Graphs/GraphsSampleSort/comp_sorted_weak_efficiency.png)
Speedup weak and strong and efficiency weak and strong plots by threads. These trends all really make a fair amount of sense with speed increasing as problem size increases for high array counts but slightly lower for low array counts. Speedup was compared based on the 2 thread case. A spike in the comm graph for the speedup is the reason for a change in the algorithm based on the processor size. Anything above 64 processes and more synchronization is needed. This showcases the loss in speedup in communication above 64 processes.

</details>


<details>
  <summary><i>Radix Sort</i></summary>

### Radix Sort Analysis: Alexander Nuccitelli
Overall radix sort ran rather slowly due to the implementation of sending each element to each process and the end of every bit sorted, which caused massive communication overhead. Normally this would be fine however MPI doesn't like sending lots of messages very quickly, so some waits had to be added, which slowed down performance. However a good thing about this implemntation is that is is quite memory efficient, only requiring to store the main array and some constants. I was unable to run this algorithim at 1024 processors as grace would error. I was also unable to run at 2^28 array size, as this would require too much computing resources.

#### Total time spent:
Overall the program decreaces as the number of processors increase which is a sign of a parrallel program. It is intresting to note that initially there is a slight increase in time between 2 and 4 and 8 processors. This is because all of those processors still need to perform all of the communication, and the program isn't parallelized enough for the extra processors to take full effect. The reason that the randomly generated array takes the least time has to do input generation. Random inputs are generated at a smaller scale, causing there to be less communication, as the array can be sorted with less bits (but still the same number of comparisions).
![main_65536](/Graphs/RadixSortGraphs/main_65536.png)
![main_262144](/Graphs/RadixSortGraphs/main_262144.png)
![main_1048576](/Graphs/RadixSortGraphs/main_1048576.png)
![main_4194304](/Graphs/RadixSortGraphs/main_4194304.png)
![main_16777216](/Graphs/RadixSortGraphs/main_16777216.png)
![main_67108864](/Graphs/RadixSortGraphs/main_67108864.png)

The comp large graphs look exactly as expected, with the number of processors massively decreasing comparision time with an exponential decrease. There is also no difference between array type becuase the comparisions are performed on all 32 bits of an integer regardless if the array is sorted or not. Comparisions also account for a very small amount of the total time spent.
![comp_large_65536](/Graphs/RadixSortGraphs/comp_large_65536.png)
![comp_large_262144](/Graphs/RadixSortGraphs/comp_large_262144.png)
![comp_large_1048576](/Graphs/RadixSortGraphs/comp_large_1048576.png)
![comp_large_4194304](/Graphs/RadixSortGraphs/comp_large_4194304.png)
![comp_large_16777216](/Graphs/RadixSortGraphs/comp_large_16777216.png)

Communication decreases roughly with processor count. The communication time also accounts for almost all of the total time spent. A randomly sorted array is faster because as the elements are sorted between arrays, they aren't moved into the same processor that they were once in if they are already sorted. Since the randomly generated elements are smaller, they finish sorting quicker and thus have to travel to less processes.
![comm_65536](/Graphs/RadixSortGraphs/comm_65536.png)
![comm_262144](/Graphs/RadixSortGraphs/comm_262144.png)
![comm_1048576](/Graphs/RadixSortGraphs/comm_1048576.png)
![comm_4194304](/Graphs/RadixSortGraphs/comm_4194304.png)
![comm_16777216](/Graphs/RadixSortGraphs/comm_16777216.png)
![comm_67108864](/Graphs/RadixSortGraphs/comm_67108864.png)

#### Strong Speedup/Weak Efficiency Plots

Ultimately, this implementation of radix sort is quite inefficient. There is some speed up in main for an increase of processors, which is a sign of a parallelized program. In addtion, the bigger the array size, the greater the speed up. This makes sense as well as the bigger array size has more elements, which leads to a greater benifit of having more processsors. However the weak efficiency graphs demonstrate that even though the algorthims do speed up, they do not speed up well with the addition of new processors. As new processors are being added, the gain in performance isn't enoguh based on the extra resources spent. There is little to no difference in input type.

![main_permuted_strong_speedup](/Graphs/RadixSortGraphs/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](/Graphs/RadixSortGraphs/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](/Graphs/RadixSortGraphs/main_random_strong_speedup.png)
![main_random_weak_efficiency](/Graphs/RadixSortGraphs/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/Graphs/RadixSortGraphs/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/Graphs/RadixSortGraphs/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/Graphs/RadixSortGraphs/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/Graphs/RadixSortGraphs/main_sorted_weak_efficiency.png)

All of the problems for main are nonexistent for the comparison region. Espically as the array size increases, the weak efficiency graphs are just straight lines, which is ideal. The speedup graphs also corespond to the number of processors much closer than the main graphs. There is no difference in input type, as input type doesn't affect the comparison regions time at all.

![comp_large_permuted_strong_speedup](/Graphs/RadixSortGraphs/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/Graphs/RadixSortGraphs/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/Graphs/RadixSortGraphs/comp_large_random_strong_speedup.png)
![comp_large_random_weak_efficiency](/Graphs/RadixSortGraphs/comp_large_random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/Graphs/RadixSortGraphs/comp_large_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/Graphs/RadixSortGraphs/comp_large_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/Graphs/RadixSortGraphs/comp_large_sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/Graphs/RadixSortGraphs/comp_large_sorted_weak_efficiency.png)

These communication region graphs almost look identical to the main graphs. This is becuase since the communication region takes up the vast majority of the time spent on the algorithm, its strong speedup and weak efficeincy graphs overpower the comparison region when combined into the main graphs.

![comm_permuted_strong_speedup](/Graphs/RadixSortGraphs/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/Graphs/RadixSortGraphs/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/Graphs/RadixSortGraphs/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/Graphs/RadixSortGraphs/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/Graphs/RadixSortGraphs/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/Graphs/RadixSortGraphs/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/Graphs/RadixSortGraphs/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/Graphs/RadixSortGraphs/comm_sorted_weak_efficiency.png)

</details>

<details>
  <summary><i>Merge Sort</i></summary>

### Merge Sort Analysis: Ariela Mitrani
This document will focus on the analysis of the parallelized merge sort algorithm that was implemented for this project using a variety of different visualizations generated from Caliper files. The report file specified the following graphs to analyze for the presentation, so the graphs I will be analyzing in this document are:

For each of comp_large, comm, and main:
- Strong scaling plots for each input_size with lines for input_type (7 plots - 4 lines each)
- Strong scaling speedup plot for each input_type (4 plots)
- Weak scaling plots for each input_type (4 plots)

Only a subset of these will be in our final presentation, but the below will include a detailed analysis on each of these groups. These were tested with array sizes 2^16, 2^18, 2^20, 2^22, 2^24, 2^26, 2^28, process numbers 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, and input types sorted, random, reverse sorted, and 1% perturbed.

Aside from the graphs below, we also recorded variance in average time per rank. It is worth noting that this variance increased with problem size for both comm and comp, because due to my implementation of merge sort, each process works at most twice (once to sort its part of the array and once to merge two sub-arrays together), so the difference in the times it takes to do these things between tasks will be greater when the difference in the array sizes each task is merging increase.

#### Main: Total Time for Program Execution
For the measurements for this section, we used Max time/rank from the Cali file, which would be the time taken by the task that does the final merge and the correctness check.

##### Strong Scaling Plots
This section has 7 graphs that span the seven different array sizes specified above, with each one having a line for each input type. the most important observations from these graphs are the following:

- The smallest array size, 65536, shows that the total program time actually increases as more processes are added. This is because the added communication overhead for additional processes heavily outweighs any benefit to computation time that those processes add on such a small problem size.
- As the array sizes increase, the point where total execution time gets worse instead of better with more processes shifts to the right (higher number of processes), and the process numbers before that point have a much steeper downward trend. This is because as the problem size increases, the value of adding processes also increases, and this break-even point is larger for larger problem sizes.
- The difference between the different input types is minimal, but the slowest is consistently random. This makes sense, as the program is not able to optimize the likely next choice as efficiently. Permuted was typically the next slowest, followed by sorted and finally reversed. The important thing to note here is that data that was random was consistently slower, as the other three times were much closer to each other than to the random time.
- As the problem size got large enough to make all the added processes helpful, the graph formed a smooth exponential decrease. We will discuss the slope of this in the following section. Another thing of note here is that the minimum time for the largest problem size (268435456) never dips below 5 seconds, suggesting that this is the amount of time that cannot be parallelized effectively.

![main_65536](/Graphs/GraphsMergeSort/main_65536.png)
![main_262144](/Graphs/GraphsMergeSort/main_262144.png)
![main_1048576](/Graphs/GraphsMergeSort/main_1048576.png)
![main_4194304](/Graphs/GraphsMergeSort/main_4194304.png)
![main_16777216](/Graphs/GraphsMergeSort/main_16777216.png)
![main_67108864](/Graphs/GraphsMergeSort/main_67108864.png)
![main_268435456](/Graphs/GraphsMergeSort/main_268435456.png)



##### Strong Speedup/Weak Efficiency Plots
These graphs show both the strong speedup and weak efficiency relative to time on two processes for all of the given input types. Each graph has a line for every input size. The observations we can make are as follows:

- There isn't a significant difference between these graphs for permuted, random, reversed, and sorted data, because they all follow the same trends even if the times differ slightly.
- The strong speedup for these graphs seems to be much closer to a linear speedup at the beginning, before leveling out to a constant or even decreasing speedup with processes added. Once again, this is because the problem size is not large enough to have a significant speedup after a certain number of processes, and the amount it speeds up starts to decrease after that point.
- Following the same reasoning, the larger problem sizes are always higher on the graph (e.g. the lines are in order of problem size). This is because for a larger problem size, the amount of speedup that is obtained by using more processors tends to be better, and that speedup doesn't hit a limit until a higher processor number (and the limit is higher).
- Looking at the weak efficiency graphs, we once again see the trend that larger problem sizes have a higher efficiency overall. However, all of the weak scaling for these processes decreases quickly until it levels out around the point where it cannot decrease any more. This relates to the slope we saw in the strong scaling plots; Even though the problem execution time decreased on a curve as the process number increased, this was not a proportional relationship (where doubling the processes halves the execution time), and thus the weak scaling efficiency drops away from 1 quickly.
![main_permuted_strong_speedup](/Graphs/GraphsMergeSort/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](/Graphs/GraphsMergeSort/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](/Graphs/GraphsMergeSort/main_random_strong_speedup.png)
![main_random_weak_efficiency](/Graphs/GraphsMergeSort/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/Graphs/GraphsMergeSort/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/Graphs/GraphsMergeSort/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/Graphs/GraphsMergeSort/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/Graphs/GraphsMergeSort/main_sorted_weak_efficiency.png)

#### Comp_Large: Average Time Spent Computing (Sorting) Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task takes to sort and merge its sections of the array.

##### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- Unlike the graphs for main, these graphs generally decrease exponentially for every problem size (with some random variations that can be caused by outside factors, like grace speed).
- These graphs follow the same trend between input types; Random is the slowest by a large margin, followed by permuted, sorted, and reversed all with close execution types.
- These graphs all tend to approach 0 even for large problem sizes, demonstrating that for large computations, there is no bottleneck added as more processes are added. If we were to include comp_small in these, there would likely be a small bottleneck but not much. In addition, these graphs seem to decrease faster than the ones for main, which we will verify in the next section by analyzing the strong speedup/weak efficiency plots.
![comp_large_65536](/Graphs/GraphsMergeSort/comp_large_65536.png)
![comp_large_262144](/Graphs/GraphsMergeSort/comp_large_262144.png)
![comp_large_1048576](/Graphs/GraphsMergeSort/comp_large_1048576.png)
![comp_large_4194304](/Graphs/GraphsMergeSort/comp_large_4194304.png)
![comp_large_16777216](/Graphs/GraphsMergeSort/comp_large_16777216.png)
![comp_large_67108864](/Graphs/GraphsMergeSort/comp_large_67108864.png)
![comp_large_268435456](/Graphs/GraphsMergeSort/comp_large_268435456.png)

##### Strong Speedup/Weak Efficiency Plots
These graphs show strong speedup and weak efficiency for the large computations in our program (such as one-processor merge sort and merging sorted arrays). We can observe the following:

- There is a strange bump on the weak efficiency graphs for 256 processors. This may be for a number of reasons, but I ran this test at a different time than some of the others so it may just have been network conditions, or it may be that the architecture distributes the processes differently for 256 processes (across nodes/tasks).
- The weak efficiency is pretty consistent and close to 1, but starts to decrease slightly as the process count gets large. This shows that at least for our computation steps, this program is pretty well parallelized (e.g. the amount of time to double the problem size with double the processors remains about constant).
- The strong speedup is close to linear, but the smaller the problem size, the more it starts to deviate from that linearity. This is  because, past a certain point, the amount of work each task does becomes so small that the added processors don't add as much efficiency. This is also why we see a slight decrease in weak efficiency with an increasing process count.
- Once again, there are no huge differences between the types of inputs for efficiency. In addition, just like for main, the larger problem sizes tend to hold the desired trends (linearity and weak efficiency = 1) better, as demonstrated by the lines being roughly in order of problem size.
![comp_large_permuted_strong_speedup](/Graphs/GraphsMergeSort/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/Graphs/GraphsMergeSort/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/Graphs/GraphsMergeSort/comp_large_random_strong_speedup.png)
![comp_large_random_weak_efficiency](/Graphs/GraphsMergeSort/comp_large_random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/Graphs/GraphsMergeSort/comp_large_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/Graphs/GraphsMergeSort/comp_large_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/Graphs/GraphsMergeSort/comp_large_sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/Graphs/GraphsMergeSort/comp_large_sorted_weak_efficiency.png)


#### Comm: Average Time Spent Communicating Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task spends sending or receiving data from other tasks.

##### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- For smaller problem sizes, the average time spent communicating increases as process count increases. However, like our graphs for main, the graphs start to show the expected trend with larger process sizes, which is a decrease in average communication time as process count increases. This seems counter-intuitive, as with more processes we would expect more communication. However, even though we do have more communication as more tasks are added, we end up having less communication time per processor. This is because the smaller operations that are present for more processes are not time-consuming enough to lead to the same amount of average work for the processes.
- A strange feature of these graphs is the sharp increase from 2 to 4 processes. This is due to the structure of the program, which makes the communication between the last two processes more ideal than other communications. Because of this, the comm time for 2 processes is lower than that for 4+ processes, and thus it deviates from the trend. If I were to change my implementation so that this wasn't the case, this would be a smooth curve all the way through -- however, the original design I had and the changes I made do not have the most optimal communication.
- Unlike main and comp, there is no relationship between input type and these graphs, which makes sense because the communication time does not consider the contents of the data it is sending, but rather just the data itself.
![comm_65536](/Graphs/GraphsMergeSort/comm_65536.png)
![comm_262144](/Graphs/GraphsMergeSort/comm_262144.png)
![comm_1048576](/Graphs/GraphsMergeSort/comm_1048576.png)
![comm_4194304](/Graphs/GraphsMergeSort/comm_4194304.png)
![comm_16777216](/Graphs/GraphsMergeSort/comm_16777216.png)
![comm_67108864](/Graphs/GraphsMergeSort/comm_67108864.png)
![comm_268435456](/Graphs/GraphsMergeSort/comm_268435456.png)

##### Strong Speedup/Weak Efficiency Plots
These graphs show strong speedup and weak efficiency for the communication between tasks in our program (such as send and receive). We can observe the following:

- Like the other strong speedup graphs, the larger the problem, the better the speedup. However, these speedup graphs are nowhere near linear, and actually seem to be pretty constant/decrease to reach zero. This is because the communication, while it takes up slightly less time on average for more processes, definitely doesn't decrease efficiently for large sizes of N (and isn't meant to). The implementation of this algorithm focused mostly on achieving strong linear speedup for the computation portion (which is the expensive part of this problem), rather than minimizing communication time.
- In the weak efficiency graphs, we see that these rapidly go to zero. Again, this is because the communication time isn't able to parallelize well for a merge sort algorithm - this is the feature that is bottlenecking the speedup of main. However, we can still observe that larger problem sizes have slightly larger values for efficiency, although not by much. This is also reflected in the strong speedup graphs, where larger problems exhibited better speedup.
![comm_permuted_strong_speedup](/Graphs/GraphsMergeSort/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/Graphs/GraphsMergeSort/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/Graphs/GraphsMergeSort/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/Graphs/GraphsMergeSort/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/Graphs/GraphsMergeSort/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/Graphs/GraphsMergeSort/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/Graphs/GraphsMergeSort/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/Graphs/GraphsMergeSort/comm_sorted_weak_efficiency.png)


#### Possible Optimizations Missed
After running all of my analysis and looking at my code structure, I determined a few optimizations that could be done to make my code more efficient. When I discussed with the professor, she said that while I didn't have to rerun my code/regenerate my graphs, I should discuss some of these optimizations in this analysis to see how I could improve my algorithm for future work. The changes I would make to optimize this are the following:

- Generate a universally sorted array (all of the data on P0 is less than all of the data on P1, etc), and implement an in-order merge that takes two sorted arrays where all of one is strictly greater than all of the other. This would make the merge step much faster if we add a condition that one array in the merge is always strictly less than the other because we would only have one comparison to do.
- To make the above optimization possible, we would also have to send the data to the same task(s) as long as possible. My current implementation sends data from two different nodes to a third node, but a more efficient way to do this would be to have the merge happen on one of the nodes with data, reducing the amount of data that needs to be sent per merge step by half. To do this, process A would merge itself with process B rather than process C merging process B and process A together.

However, aside from these optimizations, my merge sort is fairly efficient, even in the computation step which exhibits almost strong scaling. Merge sort isn't a perfectly parallelizable algorithm due to the communication overhead, but this implementation exhibits/explains the required trends.
</details>

<details>
  <summary><i>Column Sort</i></summary>
  
# Column Sort Analysis: Patralika Ghosh
My implementation of column sort closely follows Leighton’s Column Sort algorithm. The algorithm generates an array based on the specified input type, then distributes portions of the array across processes according to input size and the number of processes. Each process is responsible for sorting its assigned column and returning the sorted result. While this section of the code is parallelized, I encountered challenges with high communication overhead and synchronization, preventing me from fully parallelizing the algorithm. I experienced a number of problems as I tried to parallelize the algorithm further such as stalling at the end and Caliper files not generating properly. 

After the initial column sorting, I transpose the matrix and reshape it into submatrices of size (r/c)×r, where 
r represents the number of rows and c the number of columns. The process of column sorting is then repeated, and the matrix is transposed and reshaped back to its original configuration. Next, I introduce a shift in the matrix, setting the first half of the first column to −inf values and the last half of the last column to +inf values. After another round of column sorting, I remove the -inf and +inf values, resulting in a fully sorted matrix. The column sort algorithm only works on matrices that are not square matrices, the rows >= columns and c = 2*(columns-1)^2 and r%c == 0. Using this I tried to calculate a number that I could run for all input types, so I picked 16 since it satisfied all the above conditions. 

![16_calcs](/Images/16_calcs.png)

But as I ran my program with more number of processes, the overall runtime increased so I increased the number of columns and modified the rows accordingly. I cannot choose a high number like 512 because for 512 rows I can only run the test case with array of size 2^28. My algorithm seems to depend on how the data is distributed because I use std::sort for sorting each column locally which is easier with a small input size. 

![512_calcs](/Images/512_calcs.png)

So, these are what I ran for each test case and I kept the same configuration for each input type such as sorted, reverse sorted, 1 % perturbed and random.

![configs_1](/Images/configs_1.png)
![configs_2](/Images/configs_2.png)

I faced network errors while running the program on 1024 processors, leading to missing Caliper profiling files for this configuration. Additionally, the program times out when processing the largest array size with 32 processors or more. To improve performance, I need to enhance the parallelization of my algorithm, as column sort is inherently parallel. 


## Main: Total Time for Program Execution

For the measurements for this section, we used Max time/rank from the Cali file, which would be the time taken by the task that does is the entire algorithm.

### Strong Scaling Plots

For any input size, using fewer threads can be more efficient since the communication overhead may outweigh the benefits of parallelization, limiting any reduction in computation time. However, after 256 processors, performance improves with 512 processors, even though the overall time is still higher than with fewer processors. This suggests that increasing the number of processors beyond 256 could further enhance performance and reduce overall time.

Across nearly all input sizes and types, the overall time does not vary significantly. However, for the largest input size 2^28, a clear difference is seen: Random inputs take the longest time, followed by 1% Perturbed, Reverse, and Sorted, in decreasing order of overall time. The more disorganized an input type is, the more swapping is required, leading to an increase in overall time.

![main_65536](/Graphs/GraphsColumnSort/main_65536.png)
![main_262144](/Graphs/GraphsColumnSort/main_262144.png)
![main_1048576](/Graphs/GraphsColumnSort/main_1048576.png)
![main_4194304](/Graphs/GraphsColumnSort/main_4194304.png)
![main_16777216](/Graphs/GraphsColumnSort/main_16777216.png)
![main_67108864](/Graphs/GraphsColumnSort/main_67108864.png)
![main_268435456](/Graphs/GraphsColumnSort/main_268435456.png)

### Strong Speedup/Weak Efficiency Plots

The plots show that the algorithm's ability to scale effectively drops off as more threads are used after a initial steep incline. Speedup quickly decreases after a certain point, and the weak scaling efficiency becomes nearly zero when many threads are involved. This suggests that communication between threads becomes a major issue, slowing things down.Larger input sizes generally perform better, with higher speedups and better efficiency for a longer time, but they still struggle with performance as thread count grows. Smaller input sizes face the most trouble, even showing worse performance as the overhead from parallelization becomes too large compared to the actual computation.


![main_permuted_strong_speedup](/Graphs/GraphsColumnSort/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](/Graphs/GraphsColumnSort/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](/Graphs/GraphsColumnSort/main_random_strong_speedup.png)
![main_random_weak_efficiency](/Graphs/GraphsColumnSort/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/Graphs/GraphsColumnSort/main_reverse_strong_speedup.png)
![main_reversed_weak_efficiency](/Graphs/GraphsColumnSort/main_reverse_weak_efficiency.png)
![main_sorted_strong_speedup](/Graphs/GraphsColumnSort/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/Graphs/GraphsColumnSort/main_sorted_weak_efficiency.png)

## Comp_Small: Average Time Spent Computing (Sorting) Per Processor

For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task takes to sort and merge its sections of the array. This part of the column sort algorithm is parallelized.

### Strong Scaling Plots

For all input sizes, the time taken exponentially decreases as the number of threads increases, showing a sharp drop from 0 to 32 threads. However, the performance gains diminish beyond a certain thread count, indicating diminishing returns with higher parallelization.

For these smaller inputs, there’s a steep initial drop in time taken as threads increase from 1 to around 32, after which the time remains nearly constant. This suggests that the overhead of additional processors outweighs the benefit for smaller input sizes beyond this point.The results are consistent across different input types, meaning the algorithm's performance is relatively independent to the input type.For the largest input size, the plot only goes up to 16 threads and shows a more gradual decline in time with increasing threads. This suggests that the problem is large enough to benefit from parallelization across all available threads within this range. These graphs follow the same trend between input types; Random is the slowest by a large margin, followed by permuted, reverse and then sorted.

![comp_small_65536](/Graphs/GraphsColumnSort/comp_small_65536.png)
![comp_small_262144](/Graphs/GraphsColumnSort/comp_small_262144.png)
![comp_small_1048576](/Graphs/GraphsColumnSort/comp_small_1048576.png)
![comp_small_4194304](/Graphs/GraphsColumnSort/comp_small_4194304.png)
![comp_small_16777216](/Graphs/GraphsColumnSort/comp_small_16777216.png)
![comp_small_67108864](/Graphs/GraphsColumnSort/comp_small_67108864.png)
![comp_small_268435456](/Graphs/GraphsColumnSort/comp_small_268435456.png)

### Strong Speedup/Weak Efficiency Plots

For all input types, the speedup is close to being linear as the number of threads increases, especially for larger input sizes. Speedup improves consistently up to 512 threads, with larger input sizes achieving the highest speedup, which is expected since larger datasets benefit more from parallel processing. Past a certain point, the amount of work each task does becomes so small that the added processors don't add as much efficiency. Larger inputs generally show a better linear speedup across all thread counts, which suggests that the algorithm scales better with increasing data size. This aligns with the expectation that larger workloads offer more opportunities for parallel processing to be effective.

Weak scaling efficiency starts high with low thread counts, dips for 128 and 256 processors and then recovers as threads increase up to 512. For larger input sizes, efficiency remains closer to optimal even as the number of threads grows, especially at higher thread counts. This suggests that larger inputs handle the added overhead more effectively. The dips and peaks in the efficiency plots can be as a result of changing the rows to columns ratio or how I distributed the data in the matrix, which I added a table above of how I have been changing it.

Sorted inputs show lower efficiency at smaller input sizes across all thread counts, as the parallelization overhead outweighs any benefits when there is less work to be done. Reverse inputs maintain moderately high efficiency across all thread counts but don’t reach the peak levels of 1% Perturbed data. The Random input type shows larger drops in efficiency at 128 and 256 processor counts compared to other inputs, particularly for smaller input sizes. The 1% Perturbed input tends to maintain higher efficiency than other input types across most thread counts, especially for larger input sizes.

![comp_small_permuted_strong_speedup](/Graphs/GraphsColumnSort/comp_small_permuted_strong_speedup.png)
![comp_small_permuted_weak_efficiency](/Graphs/GraphsColumnSort/comp_small_permuted_weak_efficiency.png)
![comp_small_random_strong_speedup](/Graphs/GraphsColumnSort/comp_small_random_strong_speedup.png)
![comp_small_random_weak_efficiency](/Graphs/GraphsColumnSort/comp_small_random_weak_efficiency.png)
![comp_small_reversed_strong_speedup](/Graphs/GraphsColumnSort/comp_small_reverse_strong_speedup.png)
![comp_small_reversed_weak_efficiency](/Graphs/GraphsColumnSort/comp_small_reverse_weak_efficiency.png)
![comp_small_sorted_strong_speedup](/Graphs/GraphsColumnSort/comp_small_sorted_strong_speedup.png)
![comp_small_sorted_weak_efficiency](/Graphs/GraphsColumnSort/comp_small_sorted_weak_efficiency.png)

## Comp_Large: Average Time Spent Computing (Sorting) Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time it takes to tranpose, retranpose, shift and unshift the matrix. This part of the algorithm is sequential.

### Strong Scaling Plots
Since this part is the sequential part of the algorithm, adding thread counts should not affect the scaling, speedup or efficiency of the program. However, these graphs do not look linear as the peaks and the dips correspond to where I changed the values of my rows and columns to make my algorithm more efficient for that particular for that particular processor count. I initally intended to run everything with 16 columns and modify the columns according to the input size. But that would not give me a fair assessment for the parallelized part of the algorithm, since for any processor number above 16 would have the same performance regardless of the processor count. Here, we can see how changing the distribution of the data and rows and columns of the matrix impacted the algorithm. The trend is mostly linear except for 128 and 256 processors.

![comp_large_65536](/Graphs/GraphsColumnSort/comp_large_65536.png)
![comp_large_262144](/Graphs/GraphsColumnSort/comp_large_262144.png)
![comp_large_1048576](/Graphs/GraphsColumnSort/comp_large_1048576.png)
![comp_large_4194304](/Graphs/GraphsColumnSort/comp_large_4194304.png)
![comp_large_16777216](/Graphs/GraphsColumnSort/comp_large_16777216.png)
![comp_large_67108864](/Graphs/GraphsColumnSort/comp_large_67108864.png)
![comp_large_268435456](/Graphs/GraphsColumnSort/comp_large_268435456.png)

### Strong Speedup/Weak Efficiency Plots

As expected, there is not much of a speedup for this region, since it is the sequential region. But processors 128 and 256 seem to have similar behavior for input sizes and types except for the second largest input size 2^26, that could be because the distribution of the data was optimal so the speedup is mostly a straight line. Weak effienciency graphs results sharply decreases to zero since with adding more processors has no impact on the efficiency of the algorithm since this part of the algorithm is not parallelized.

![comp_large_permuted_strong_speedup](/Graphs/CorrectGraphsColumn/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/Graphs/CorrectGraphsColumn/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/Graphs/CorrectGraphsColumn/comp_large_Random_strong_speedup.png)
![comp_large_random_weak_efficiency](/Graphs/CorrectGraphsColumn/comp_large_Random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/Graphs/CorrectGraphsColumn/comp_large_Reverse_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/Graphs/CorrectGraphsColumn/comp_large_Reverse_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/Graphs/CorrectGraphsColumn/comp_large_Sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/Graphs/CorrectGraphsColumn/comp_large_Sorted_weak_efficiency.png)

## Comm: Average Time Spent Communicating Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task spends sending or receiving data from other tasks.

### Strong Scaling Plots

For smaller input sizes, as more threads are added, the execution time increases steadily, showing that the algorithm doesn’t scale well. The extra work required to manage multiple threads isn’t worth it, and the algorithm might actually run faster with fewer processes. For larger input sizes, the algorithm shows good performance improvement up to 4 threads, but after that, the gains level off. This indicates that while the algorithm is suited for bigger datasets, there’s a point where the communication overhead becomes too much as more threads are added. From the other graphs above, it seems for larger datasets the distribution of data was more uniform and favourable for a larger process count than smaller process count. So after 256 processes there seems to be an improvement in the overall time and scability of the algorithm.

![comm_65536](/Graphs/GraphsColumnSort/comm_65536.png)
![comm_262144](/Graphs/GraphsColumnSort/comm_262144.png)
![comm_1048576](/Graphs/GraphsColumnSort/comm_1048576.png)
![comm_4194304](/Graphs/GraphsColumnSort/comm_4194304.png)
![comm_16777216](/Graphs/GraphsColumnSort/comm_16777216.png)
![comm_67108864](/Graphs/GraphsColumnSort/comm_67108864.png)
![comm_268435456](/Graphs/GraphsColumnSort/comm_268435456.png)

### Strong Speedup/Weak Efficiency Plots

The algorithm struggles to scale effectively as more threads are used. Speedup and efficiency drop quickly, especially when using more than 8 threads, regardless of the input size. This suggests that the current parallelization approach may not be well-suited. Larger input sizes perform better at first, showing higher speedup and efficiency, which indicates that the algorithm works best when each thread has more work to do. This minimizes the effect of communication overhead. However, even with larger input sizes, the benefits of adding more threads quickly decrease as the thread count rises.

![comm_permuted_strong_speedup](/Graphs/GraphsColumnSort/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/Graphs/GraphsColumnSort/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/Graphs/GraphsColumnSort/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/Graphs/GraphsColumnSort/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/Graphs/GraphsColumnSort/comm_reverse_strong_speedup.png)
![comm_reversed_weak_efficiency](/Graphs/GraphsColumnSort/comm_reverse_weak_efficiency.png)
![comm_sorted_strong_speedup](/Graphs/GraphsColumnSort/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/Graphs/GraphsColumnSort/comm_sorted_weak_efficiency.png)

</details>

<details>
  <summary><i>Bitonic Sort</i></summary>
  
  # Bitonic Sort Analysis: Maximiliano Pombo
This document will focus on the analysis of the parallelized bitonic sort algorithm that was implemented for this project using a variety of different visualizations generated from Caliper files. The report file specified the following graphs to analyze for the presentation, so the graphs I will be analyzing in this document are:

Only a subset of these will be in our final presentation, but the below will include a detailed analysis on each of these groups. These were tested with array sizes 2^16, 2^18, 2^20, 2^22, 2^24, 2^26, 2^28, process numbers 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, and input types sorted, random, reverse sorted, and 1% perturbed.


## Main: Total Time for Program Execution
For the measurements for this section, we used Max time/rank from the Cali file, which would be the time taken by the task that does the final merge and the correctness check.

### Strong Scaling Plots
This section has 6 graphs that span the seven different array sizes specified above, with each one having a line for each input type. the most important observations from these graphs are the following:

- The smallest array size, 65536, shows that the total program time actually increases as more processes are added. This is because the added communication overhead for additional processes heavily outweighs any benefit to computation time that those processes add on such a small problem size.
- For all input sizes, the type of input (sorted, reverse, random, perturbed) didn't have a large effect on performance. This is due to the way bitonic sort works, as it is an oblivious sorting algorithm that performs the same sequence of comparisons and swaps regardless of the initial order of the elements. Bitonic sort follows a fixed pattern of merging and sorting phases, making it insensitive to input distribution and maintaining consistent performance across different input types.
- For input sizes "262144, 1048576, 4194304, 16777216" we see an increase in time when using 128 processors. This might be due to the overhead associated with managing and synchronizing a larger number of processors. As the number of processors increases, communication and coordination between processors become more complex and can introduce latency, especially if the problem size is not large enough to efficiently utilize all the processors.
- As the problem size got large enough to make all the added processes helpful, the graph formed a smooth exponential decrease for all input types.

![main_65536](/Graphs/CorrectGraphsBitonic/main_65536.png)
![main_262144](/Graphs/CorrectGraphsBitonic/main_262144.png)
![main_1048576](/Graphs/CorrectGraphsBitonic/main_1048576.png)
![main_4194304](/Graphs/CorrectGraphsBitonic/main_4194304.png)
![main_16777216](/Graphs/CorrectGraphsBitonic/main_16777216.png)
![main_67108864](/Graphs/CorrectGraphsBitonic/main_67108864.png)

### Strong Speedup/Weak Efficiency Plots
These graphs show both the strong speedup and weak efficiency relative to time on two processes for all of the given input types. Each graph has a line for every input size. The observations we can make are as follows:

- There isn't a significant difference between these graphs for permuted, random, reversed, and sorted data, because they all follow the same trends even if the times differ slightly.
- The bitonic sort algorithm demonstrates significantly higher speedup with larger input sizes, peaking around 9 for the largest dataset (26,843,546) in random input scenarios, while smaller datasets yield lower speedup values.
- Across various input types (1% perturbed, random, reverse sorted, and sorted), the algorithm maintains a robust performance, with maximum speedup levels generally ranging from 6 to 8, indicating its effectiveness in a parallel processing environment.
- Weak efficiency sharply decreases as the number of threads increases, approaching zero at higher thread counts, highlighting that simply adding threads does not equate to effective utilization of computational resources.

![main_permuted_strong_speedup](/Graphs/CorrectGraphsBitonic/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](/Graphs/CorrectGraphsBitonic/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](/Graphs/CorrectGraphsBitonic/main_Random_strong_speedup.png)
![main_random_weak_efficiency](/Graphs/CorrectGraphsBitonic/main_Random_weak_efficiency.png)
![main_reversed_strong_speedup](/Graphs/CorrectGraphsBitonic/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/Graphs/CorrectGraphsBitonic/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/Graphs/CorrectGraphsBitonic/main_Sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/Graphs/CorrectGraphsBitonic/main_Sorted_weak_efficiency.png)

## Comp_Large: Average Time Spent Computing (Sorting) Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task takes to sort and merge its sections of the array.

### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- Unlike the graphs for main, these graphs generally decrease exponentially for every problem size 
- For larger input sizes (like 4,194,304 and 16,777,216), the time reduction begins to plateau as the number of threads approaches 1,000, suggesting diminishing returns for adding more threads beyond a certain point.
- These graphs follow the same trend between input types where the input type seems to have little to no effect on the execution times.
- These graphs all tend to approach 0 even for large problem sizes, demonstrating that for large computations, there is no bottleneck added as more processes are added. If we were to include comp_small in these, there would likely be a small bottleneck but not much. In addition, these graphs seem to decrease faster than the ones for main, which we will verify in the next section by analyzing the strong speedup/weak efficiency plots.

![comp_large_65536](/Graphs/CorrectGraphsBitonic/comp_65536.png)
![comp_large_262144](/Graphs/CorrectGraphsBitonic/comp_262144.png)
![comp_large_1048576](/Graphs/CorrectGraphsBitonic/comp_1048576.png)
![comp_large_4194304](/Graphs/CorrectGraphsBitonic/comp_4194304.png)
![comp_large_16777216](/Graphs/CorrectGraphsBitonic/comp_16777216.png)
![comp_large_67108864](/Graphs/CorrectGraphsBitonic/comp_67108864.png)

### Strong Speedup/Weak Efficiency Plots
These graphs show strong speedup and weak efficiency for the large computations in our program. We can observe the following:

- The bitonic sort algorithm exhibits good speedup across various input sizes, indicating its robust performance in parallel processing environments, particularly with larger datasets.
- All input types (1% perturbed, random, reverse sorted, and sorted) show similar speedup trends, suggesting that the algorithm effectively harnesses available threads regardless of data arrangement.
- Weak efficiency remains around 1.0 to 1.1 for most thread counts, demonstrating efficient utilization of computational resources; however, occasional dips highlight variability in performance, particularly at certain thread counts.
- The minor fluctuations in weak efficiency across different input types indicate that while the algorithm generally scales well, there are challenges related to workload management and thread utilization that could be optimized further.

![comp_large_permuted_strong_speedup](/Graphs/CorrectGraphsBitonic/comp_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/Graphs/CorrectGraphsBitonic/comp_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/Graphs/CorrectGraphsBitonic/comp_Random_strong_speedup.png)
![comp_large_random_weak_efficiency](/Graphs/CorrectGraphsBitonic/comp_Random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/Graphs/CorrectGraphsBitonic/comp_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/Graphs/CorrectGraphsBitonic/comp_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/Graphs/CorrectGraphsBitonic/comp_Sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/Graphs/CorrectGraphsBitonic/comp_Sorted_weak_efficiency.png)


## Comm: Average Time Spent Communicating Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task spends sending or receiving data from other tasks.

### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- Across all input sizes, communication time drops significantly as the number of threads increases, particularly noticeable from 0 to about 200 threads, where there is a sharp decline.
- For larger thread counts (beyond 200), the communication time stabilizes at very low levels (close to 0 seconds), suggesting that increasing the number of threads further does not significantly impact the communication time due to efficient handling of data in parallel.
- The communication time analysis of the bitonic sort algorithm reveals that it scales effectively with increasing thread counts, demonstrating a significant reduction in overhead as the workload is distributed. This efficiency is consistent across various input sizes and types, suggesting that the algorithm is well-optimized for parallel processing environments, even as input complexity increases.

![comm_65536](/Graphs/CorrectGraphsBitonic/comm_65536.png)
![comm_262144](/Graphs/CorrectGraphsBitonic/comm_262144.png)
![comm_1048576](/Graphs/CorrectGraphsBitonic/comm_1048576.png)
![comm_4194304](/Graphs/CorrectGraphsBitonic/comm_4194304.png)
![comm_16777216](/Graphs/CorrectGraphsBitonic/comm_16777216.png)
![comm_67108864](/Graphs/CorrectGraphsBitonic/comm_67108864.png)

### Strong Speedup/Weak Efficiency Plots
These graphs show strong speedup and weak efficiency for the communication between tasks in our program (such as send and receive). We can observe the following:

- The graphs show a sharp increase in speedup at lower thread counts, particularly for input types with smaller sizes (e.g., 65536 and 262144). This suggests that increasing the number of threads significantly enhances performance up to a certain point, highlighting the effectiveness of parallel processing in handling smaller data sets.
- The weak efficiency graphs demonstrate an initial spike in efficiency at low thread counts, followed by a sharp decline as the number of threads increases. This indicates that while more threads can improve speed, the efficiency per thread drops significantly, particularly evident with larger input sizes.
- The results indicate a clear trade-off between speedup and weak efficiency. While utilizing more threads can lead to substantial speed improvements, it often comes at the cost of efficiency. This underscores the importance of optimizing thread usage and considering input characteristics to achieve the best performance in parallel algorithms. 

![comm_permuted_strong_speedup](/Graphs/CorrectGraphsBitonic/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/Graphs/CorrectGraphsBitonic/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/Graphs/CorrectGraphsBitonic/comm_Random_strong_speedup.png)
![comm_random_weak_efficiency](/Graphs/CorrectGraphsBitonic/comm_Random_weak_efficiency.png)
![comm_reversed_strong_speedup](/Graphs/CorrectGraphsBitonic/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/Graphs/CorrectGraphsBitonic/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/Graphs/CorrectGraphsBitonic/comm_Sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/Graphs/CorrectGraphsBitonic/comm_Sorted_weak_efficiency.png)


### Overall performance
The bitonic sort algorithm is well-suited for parallelization because it inherently breaks down the data into independent subproblems that can be sorted separately. While it struggled with smaller input sizes and did not benefit from parallel processing, its performance significantly improved as the input size increased, demonstrating its effective use of parallelism for larger datasets.


## Possible Optimizations Missed
After running all of my analysis and looking at my code structure, I determined a few optimizations that could be done: 
- Dynamic Load Balancing and Thread Pooling: Implementing dynamic load balancing can ensure even distribution of workloads across threads, while using thread pooling can reduce the overhead of thread creation and destruction, enhancing overall performance.
- Minimizing Communication Overhead and Cache Optimization: Strategies to reduce inter-thread communication, such as shared memory usage, and optimizing memory access patterns to improve cache locality can significantly lower latency and improve sorting efficiency.
- Algorithmic Refinements and Utilization of Modern CPU Capabilities: Exploring hybrid sorting techniques, algorithmic adaptations based on input characteristics, and employing SIMD (Single Instruction, Multiple Data) instructions can enhance performance by better leveraging available computational resources.


</details>

## 5. Presentation
Plots for the presentation should be as follows:
- For each implementation:
    - For each of comp_large, comm, and main:
        - Strong scaling plots for each input_size with lines for input_type (7 plots - 4 lines each)
        - Strong scaling speedup plot for each input_type (4 plots)
        - Weak scaling plots for each input_type (4 plots)

Analyze these plots and choose a subset to present and explain in your presentation.

<details>
  <summary><i>Overall Algorithm Comparison</i></summary>
  
  ### Main Execution Time

  Overall, each of the different algorthims had wildly different execution times. Radix sort was by far the slowest algorithm as it had quite a lot of communication overhead due to sending each element invduvally to each processor. The graph shown below is with radix sort.
  
  ![overall graph with radix sort](/Graphs/ComparisonGraphs/OverallGraphWithRadixSort.jpg)

  This is the same graph shown above but without radix sort, so other trends in algorthims can be viewed more clearly. Merge sort and sample sort perform by far the best, as they take limited time, even at lower processor counts. Intrestingly, bitonic sort intially takes more time than column sort, however column sort increases with the number of processors while bitonic sort decreases with the number of processors. The expected behavior for all algortihms is that as you add more processors, the execution time decreases. The reason the column sort doesn't display this property is that the communcation between processes increases faster than the comparison benifit for including more processors.

  ![overall graph without radix sort](/Graphs/ComparisonGraphs/OverallGraphWithoutRadixSort.jpg)  

</details>

<details>
  <summary><i>Overall Scaling Comparison</i></summary>
  
  ### Overall Scaling Comparison

  Although the details on why the individual lines on these graphs look the way they do are included in the individual algorithm analysis shown above, there are some details for these graphs that can be touched on. 

  First of all, looking at the computation time, it may appear strange that the column sort computation is linear scaling. However, this graph only looks at the parallelized part of column sort, which explains this. Furthermore, it may be surprising that sample sort exhibits weak computation time and ends up having the best scaling for the whole program. However, this is because the communication in sample sort is more efficient and does not detract much from the computation speedup, unlike with the other algorithms. We could also possibly get more trend information from more processes to see how the behavior of sample sort compares to the other algorithms as it grows, but due to limitations with execution time, we could not get this data.
  
  ![speedupmain](/Graphs/ComparisonGraphs/strongmain.png)
  ![weakefficiencymain](/Graphs/ComparisonGraphs/weakmain.png)
  ![speedupcomp](/Graphs/ComparisonGraphs/strongcomp.png)
  ![weakefficiencycomp](/Graphs/ComparisonGraphs/weakcomp.png)

</details>

<details>
  <summary><i>Communication Analysis</i></summary>
  
### Communication Time

The overall comm time for radix sort is significantly higher because of how radix sort was implemented. Radix sort uses bitwise sorting with a base of 32, so the sorting is complete only after 32 iterations. Also, after sorting each element is sent to the new araay individually as a result of which the number of MPI_send calls that are created are 32*array_size. Column sort is not properly or efficiently parallelized and it uses std::sort to sort each column which has an affect on the overall communication time as the numerb of processors increases.

![comm_time_overall](/Images/comm_time_overall.png)

For the speedup graph, radix and merge have a linear speedup which is a good sign but for merge sort it reaches a saturation point after which the speedup decreases. Sample sort is supposed to be the best in communication time and worst for computation time. However, the sample sort trend for the speedup graph is similar to bitonic sort and column osrt which suggests that implementation of smaple sort was not efficient. For column sort and bitonic sort, the algorithm is not parallelized properly and with that there is a exponential decay in their speedup.

![comm_time_speedup](/Images/comm_time_speedup.png)

None of the algorithms are able to parallelize well for communication time. As a result of which, the efficiency trends rapidly go to zero.
![comm_time_efficiency](/Images/comm_time_efficiency.png)

</details>

<details>
  <summary><i>Cache Miss Analysis</i></summary>
  
  ![L1Miss](/Graphs/CacheMissL1.png)
  ![L2Miss](/Graphs/CacheMissL2.png)
  Above are the L1 and L2 cache miss graphs for our sorting algorithms. Merge sort is not present since the creator is not part of the honors section. The most jarring thing for the above graphs is the order of magnitude. This is because of the time it takes to run the algorithms. Since sample sort takes less time than the other algorithms so it makes sense that it would be less. Column sort is by far the least optimized for the cache miss due to the large amount of L2 misses present and the fact that it runs significantly faster than radix sort despite having more L2 misses than it and comparable L1 misses. Ideal trends involve the cache misses decreasing as the processes increase since each process will have less data. This is not always the case. 
    ![RadixMiss](/Graphs/cacheRadix.png)
  ![SampleMiss](/Graphs/CacheSample.png)
    ![ColumnMiss](/Graphs/CacheColumn.png)
The cache miss has some effect on input type. For radix there is little effect which makes sense based on how the input types work for that algorithm. Column sort and Sample sort both use STD sort which has an effect on cache misses accordingly.

</details>

## 6. Final Report
Submit a zip named `TeamX.zip` where `X` is your team number. The zip should contain the following files:
- Algorithms: Directory of source code of your algorithms.
- Data: All `.cali` files used to generate the plots seperated by algorithm/implementation.
- Jupyter notebook: The Jupyter notebook(s) used to generate the plots for the report.
- Report.md



