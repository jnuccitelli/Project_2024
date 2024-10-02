# CSCE 435 Group project

## 0. Group number: 

## 1. Group members:
1. Ariela Mitrani
2. Joseph Nuccitelli
3. Alexander Nuccitelli
4. Patralika Ghosh
5. Maximiliano Pombo

## 2. Project topic (e.g., parallel sorting algorithms)
Parallel sorting algorithms

### 2a. Project communication
For this project, our team will communicate via Discord for planning, organization, and collaboration.

### 2b. Brief project description (what algorithms will you be comparing and on what architectures)
For this project, we will be comparing the following 5 algorithms:
- Bitonic Sort
- Sample Sort (Joseph)
- Merge Sort (Ariela)
- Radix Sort (Alex)
- Column Sort
We will be implementing everything with MPI for this project.

### 2c. Pseudocode for each parallel algorithm
- For MPI programs, include MPI calls you will use to coordinate between processes
#### Bitonic Sort:
#### Sample Sort: Joseph
#### Merge Sort: Ariela
#### Radix Sort: Alex
#### Column Sort:

### 2d. Evaluation plan - what and how will you measure and compare
- Compare different algorithms to sort and see how the performance differs across them
- Compare communication strategies; Collective vs point-to-point
- Compare different parallelization strategies (master/worker vs SPMD)
- Compare different input sizes with both strong and weak scaling (so increasing the input size with and without increasing the number of processes)
- Compare different input types; Randomly generated, already sorted, sorted in reverse, sorted with 1% permuted
