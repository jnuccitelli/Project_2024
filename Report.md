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

### 2d. Evaluation plan - what and how will you measure and compare
- Compare different algorithms to sort and see how the performance differs across them
- Compare communication strategies; Collective vs point-to-point
- Compare different parallelization strategies (master/worker vs SPMD)
- Compare different input sizes with both strong and weak scaling (so increasing the input size with and without increasing the number of processes)
- Compare different input types; Randomly generated, already sorted, sorted in reverse, sorted with 1% permuted

## 3. Descriptions of Implemented Algorithms
In this section, we will discuss the ways we implemented our various sorting algorithms, such as how they communicate, how the computation workload is split, etc.

### Merge Sort - Ariela
My merge sort implementation was similar to my pseudocode, but after running analysis on the algorithm, I realized that I wasn't using the processors as efficiently as I could have. To fix this, I distributed the work evenly over all of the nodes by having them each generate their own data and sort it, then used the original parent-child hierarchy to determine which nodes should be executing the merge. Essentially, every node starts with an array of length totalArraySize/numNodes, sorts that, and then sends the sorted array to a specific task that will be responsible for merging the arrays together. Thus, in every step there are half the amount of processes working as there were in the previous step, which is the maximum amount of processes that can be working at one time for merge sort. 

Thicket tree:
![merge_thicket_tree](ThicketTrees/mergethicket.png)

### Radix Sort - Alex
This implmentation of radix sort only works for integers. It first generates arrays in each process. Each process has one segement of the array. After that it performs a local counting sort on the least significant bit of each integer. It also gets the local number of 0 bits, which come before the 1 bits. After sorting locally, the local number of 0's and 1's is distributed accordingly such that each processor has the total number of 0 bits, as well as the amount of 0 and 1 bits present in previous processors. This allows each processor to get the location of the sorted element. This has to be a stable sort, since otherwise the order would be messed up. Finally each element is moved to it's new position in reation to the total array. Intially in the psuedocode the implmentation used 1 sided communication using MPI_Put, however even though this would improve performance, it kept writing values to incorrect spots in memory. The actual implmenation uses 2 sided communication. The correctness checking first checks each array, and then checks the last element of the previous array with the first element of the next one.

Call Tree:
![image](https://github.com/user-attachments/assets/271a69e5-be2c-47c4-9b50-17839adb1e19)
Meta Data:
![image](https://github.com/user-attachments/assets/eaaafc7e-c83b-456a-8bac-c42159de99d5)



### Bitonic Sort - Maximiliano 
My implementation of the bitonic sort algorithm closely followed the pseudocode, but I made several optimizations to ensure efficient use of the available processors. Each processor begins by generating its own segment of the array, with the size of each segment determined by the total array size divided by the number of processors. This approach allows each node to perform a local bitonic sort on its chunk of the array independently.
The bitonic sort operates in multiple stages. In each stage, the processors first determine the sorting direction based on their rank and the current step size. This hierarchical structure allows for efficient merging of the sorted segments, where pairs of processors collaborate to perform the bitonic merge operation. By dynamically adjusting the step size and merging the results, I ensured that the number of active processors reduces by half in each subsequent step, maximizing parallelism during the sorting process.

Call Tree:
![column_thicket_tree](/ThicketTrees/BitonicTree.png)
Meta Data:
![column_metadata](/Metadata/BitonicMetadata.png)


### Column Sort - Patralika Ghosh
My implementation of the column sort closely follows the structure of my pseudocode and is largely based on Leighton's Column Sort algorithm. The algorithm generates the array according to the input type, and each process is assigned a portion of indexes based on the input size and the number of processes. Each process then sorts its assigned column and returns the sorted result. Next, I transpose the matrix with the sorted columns and reshape it into submatrices of size (r/c) X r, where r is the number of rows and c is the number of columns.
I repeat the process of column sorting, transposing, and reshaping two more times. Afterward, I perform a shift in the matrix by adding an extra column, where one half is filled with -inf values and the other half with +inf values. I sort the columns again, then remove the -inf and +inf values, resulting in a fully column-sorted matrix.


![column_thicket_tree](/ThicketTrees/columnthicket.png)
![column_metadata](/Metadata/columnMetadata.png)

### Sample Sort - Joseph Nuccitelli
First each process in my code generates their local splitters. They do this by sorting their local data and sampling splitters. Then each process sends their local splitters to the main process. The main process then sorts all of the local samples and creates global splitters. These are then broadcasted out to everyone. Then each process puts each element into the array into a bucket. The last part each process is given a bucket. They copy their given bucket and then send other processes buckets to each process. After that each process sorts their new bucket and we have a sorted array.

![image](https://github.com/user-attachments/assets/568b0958-94ce-4325-8a2c-3c12047e5180)

## 4. Performance evaluation

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


## 5. Presentation
Plots for the presentation should be as follows:
- For each implementation:
    - For each of comp_large, comm, and main:
        - Strong scaling plots for each input_size with lines for input_type (7 plots - 4 lines each)
        - Strong scaling speedup plot for each input_type (4 plots)
        - Weak scaling plots for each input_type (4 plots)

Analyze these plots and choose a subset to present and explain in your presentation.

## 6. Final Report
Submit a zip named `TeamX.zip` where `X` is your team number. The zip should contain the following files:
- Algorithms: Directory of source code of your algorithms.
- Data: All `.cali` files used to generate the plots seperated by algorithm/implementation.
- Jupyter notebook: The Jupyter notebook(s) used to generate the plots for the report.
- Report.md



