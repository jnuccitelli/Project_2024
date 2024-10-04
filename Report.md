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

int boredProcesses = procNum;

for(n in procNum) {
  if(taskid == n) {
    int children = min(boredProcesses, 2);
    boredProcesses -= children;

    int[] array;
    if(taskid == 0)
      array = generateArray(arraySize); //this fills our array with values
      startChildProcesses(n, children, array, arraySize)
    else
      MPI_recv(array from parent process)
      MPI_send(startChildProcesses(n, children, array, arraySize) to our parent task (id-1 if even, id-2 if odd))
  }
}

if(taskid == 0)
  MPI_recv(finalArray)

printf(finalArray)
return; //end of program main

//helper function 1
array startChildProcesses(myId, numChildren, array, arraySize) {
  if(numChildren == 0)
    return mergeSort(array, arraySize)
  if(numChildren == 1)
    MPI_send(left half of array and size to myId+1)
    sortedRight = mergeSort(right half of array, arraySize/2)
    sortedLeft; //empty array
    MPI_recv(sortedLeft from myId+1)
    return combineSortedArrays(sortedRight, sortedLeft)
  if(numChildren == 2)
    MPI_send(left half of array and size to myId+1)
    MPI_send(right half of array and size to myId+2)
    sortedLeft; //empty array
    sortedRight; //empty array
    MPI_recv(sortedLeft from myId+1)
    MPI_recv(sortedRight from myId+2)
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
int arraySize = user_input_for_array_size;
int procNum;
int taskId;

MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
MPI_Comm_size(MPI_COMM_WORLD, &procNum);

int boredProcesses = procNum;
int numRows = sqrt(procNum);  


int arrayDimension = sqrt(arraySize);

// Create 2D array for local process
int[][] localArray;

for (int n in procNum) {
    if (taskId == n) {
        int children = min(boredProcesses, 2);
        boredProcesses -= children;

        if (taskId == 0) {
            
            int[] array = generateArray(arraySize);  
            int[][] array2D = convertTo2D(array, arrayDimension);  

            // Split 2D array rows and distribute them to child processes
            startChildProcesses(n, children, array2D, arrayDimension);
        } else {
            MPI_recv(localArray from parent process);
            MPI_send(startChildProcesses(n, children, localArray, arrayDimension) to parent task (id - 1 if even, id - 2 if odd));
        }
    }
}

// Root process collects sorted data from child processes
if (taskId == 0) {
    MPI_recv(finalArray);
}

printf(finalArray);
return;

// Split 2D array rows and distribute them to child processes
array startChildProcesses(myId, numChildren, array, arraySize) {
    if (numChildren == 0) {
        return columnSort(array, arraySize);  
    }
    if (numChildren == 1) {
        MPI_send(left half of array and size to myId + 1);
        sortedRight = columnSort(right half of array, arraySize / 2);
        sortedLeft;  
        MPI_recv(sortedLeft from myId + 1);
        return combineSortedArrays(sortedRight, sortedLeft);
    }
    if (numChildren == 2) {
        MPI_send(left half of array and size to myId + 1);
        MPI_send(right half of array and size to myId + 2);
        sortedLeft; 
        sortedRight;  
        MPI_recv(sortedLeft from myId + 1);
        MPI_recv(sortedRight from myId + 2);
        return combineSortedArrays(sortedRight, sortedLeft, arraySizeRight, arraySizeLeft);
    }
}

// Helper function 2: Column Sort algorithm
int[][] columnSort(int[][] array2D, int dimension) {
    // Sort columns
    for (int i = 0; i < dimension; i++) {
        array2D[i] = sequentialSort(array2D[i]);  
    }

    //Perform a row permutation step
    array2D = rowPermutation(array2D, dimension);

    // Sort each row
    for (int i = 0; i < dimension; i++) {
        array2D[i] = sequentialSort(array2D[i]);  // Sort each row
    }

    // Perform a column permutation step
    array2D = columnPermutation(array2D, dimension);

    // Sort columns again
    for (int i = 0; i < dimension; i++) {
        array2D[i] = sequentialSort(array2D[i]);  // Final column sort
    }

    return array2D;
}

// Perform row permutation
int[][] rowPermutation(int[][] array2D, int dimension) {
    int temp[dimension];
    
    // Shuffle rows within the process
    for (int i = 0; i < dimension; i++) {
        int newPos = (i + 1) % dimension;
        temp[newPos] = array2D[i];
    }
    
    // Copy permuted rows back to array2D
    for (int i = 0; i < dimension; i++) {
        array2D[i] = temp[i];
    }
    
    // If rows are split across processes:
    if (numProcesses > 1) {
        // Send/receive rows to/from other processes
        MPI_Sendrecv_replace(&array2D, dimension * sizeof(int), MPI_INT, 
                             neighborProcessID, 0, neighborProcessID, 0, MPI_COMM_WORLD, &status);
    }

    return array2D;
}


// Perform column permutation
int[][] columnPermutation(int[][] array2D, int dimension) {
    int temp[dimension];
    
    // Shuffle columns within the process
    for (int i = 0; i < dimension; i++) {
        int newPos = (i + 1) % dimension;  
        for (int j = 0; j < dimension; j++) {
            temp[j] = array2D[j][i];  
        }
        for (int j = 0; j < dimension; j++) {
            array2D[j][newPos] = temp[j];  
        }
    }
    
    // MPI Communication for distributed columns
    // If columns are split across processes:
    if (numProcesses > 1) {
        MPI_Sendrecv_replace(&array2D, dimension * sizeof(int), MPI_INT, 
                             neighborProcessID, 0, neighborProcessID, 0, MPI_COMM_WORLD, &status);
    }

    return array2D;
}


// Combine sorted arrays from child processes
int[] combineSortedArrays(int[] sortedRight, int[] sortedLeft, int sizeRight, int sizeLeft) {
    int[] returnArray = new int[sizeRight + sizeLeft];
    int r = 0;
    int l = 0;
    for (int i = 0; i < sizeRight + sizeLeft; i++) {
        if (r == sizeRight) {
            returnArray[i] = sortedLeft[l];
            l++;
        } else if (l == sizeLeft) {
            returnArray[i] = sortedRight[r];
            r++;
        } else if (sortedRight[r] < sortedLeft[l]) {
            returnArray[i] = sortedRight[r];
            r++;
        } else {
            returnArray[i] = sortedLeft[l];
            l++;
        }
    }
    return returnArray;
}

```

### 2d. Evaluation plan - what and how will you measure and compare
- Compare different algorithms to sort and see how the performance differs across them
- Compare communication strategies; Collective vs point-to-point
- Compare different parallelization strategies (master/worker vs SPMD)
- Compare different input sizes with both strong and weak scaling (so increasing the input size with and without increasing the number of processes)
- Compare different input types; Randomly generated, already sorted, sorted in reverse, sorted with 1% permuted
