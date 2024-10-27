# Column Sort Analysis: Patralika Ghosh
My implementation of the column sort closely follows and is largely based on Leighton's Column Sort algorithm. The algorithm generates the array according to the input type, and each process is assigned a portion of indexes based on the input size and the number of processes. Each process then sorts its assigned column and returns the sorted result. This part of the code is parallelized, however due to high communication overhead I was not able to parallelize the entire algorithm properly. Next, I transpose the matrix with the sorted columns and reshape it into submatrices of size (r/c) x r, where r is the number of rows and c is the number of columns. I repeat the process of column sorting and re-tranpose the matrix back to its original form. Afterward, I perform a shift in the matrix, where one half of the first column is filled with -inf values and the other half of the last column is filled with +inf values. I sort the columns again, then remove the -inf and +inf values, resulting in a fully column-sorted matrix.
Since my implementation lacks sufficient parallelization, I am missing a couple of Caliper files because of network errors that I faced while trying to run my program for 1024 and 512 number of processors. My program is timing out for the largest array size for processors 32 and onwards and when it is run on number of processes for 512 and 1024. 

The way I could improve the performance of my algorithm is to try to completely parallelize my algorithm since column sort is a parallel algorithm. Increasing the amount of parallelization of my algorithm will help me reduce the overall computation time of my algorithm.


## Main: Total Time for Program Execution
For the measurements for this section, we used Max time/rank from the Cali file, which would be the time taken by the task that does the final merge and the correctness check.

### Strong Scaling Plots

![main_65536](../Graphs/GraphsColumnSort/main_65536.png)
![main_262144](../Graphs/GraphsColumnSort/main_262144.png)
![main_1048576](../Graphs/GraphsColumnSort/main_1048576.png)
![main_4194304](../Graphs/GraphsColumnSort/main_4194304.png)
![main_16777216](../Graphs/GraphsColumnSort/main_16777216.png)
![main_67108864](../Graphs/GraphsColumnSort/main_67108864.png)
![main_268435456](../Graphs/GraphsColumnSort/main_268435456.png)

### Strong Speedup/Weak Efficiency Plots

![main_permuted_strong_speedup](../Graphs/GraphsColumnSort/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](../Graphs/GraphsColumnSort/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](../Graphs/GraphsColumnSort/main_random_strong_speedup.png)
![main_random_weak_efficiency](../Graphs/GraphsColumnSort/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](../Graphs/GraphsColumnSort/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](../Graphs/GraphsColumnSort/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](../Graphs/GraphsColumnSort/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](../Graphs/GraphsColumnSort/main_sorted_weak_efficiency.png)

## Comp_Small: Average Time Spent Computing (Sorting) Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task takes to sort and merge its sections of the array.
### Strong Scaling Plots

![comp_large_65536](../Graphs/GraphsColumnSort/comp_large_65536.png)
![comp_large_262144](../Graphs/GraphsColumnSort/comp_large_262144.png)
![comp_large_1048576](../Graphs/GraphsColumnSort/comp_large_1048576.png)
![comp_large_4194304](../Graphs/GraphsColumnSort/comp_large_4194304.png)
![comp_large_16777216](../Graphs/GraphsColumnSort/comp_large_16777216.png)
![comp_large_67108864](../Graphs/GraphsColumnSort/comp_large_67108864.png)
![comp_large_268435456](../Graphs/GraphsColumnSort/comp_large_268435456.png)

### Strong Speedup/Weak Efficiency Plots

![comp_large_permuted_strong_speedup](../Graphs/GraphsColumnSort/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](../Graphs/GraphsColumnSort/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](../Graphs/GraphsColumnSort/comp_large_random_strong_speedup.png)
![comp_large_random_weak_efficiency](../Graphs/GraphsColumnSort/comp_large_random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](../Graphs/GraphsColumnSort/comp_large_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](../Graphs/GraphsColumnSort/comp_large_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](../Graphs/GraphsColumnSort/comp_large_sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](../Graphs/GraphsColumnSort/comp_large_sorted_weak_efficiency.png)


## Comm: Average Time Spent Communicating Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task spends sending or receiving data from other tasks.

### Strong Scaling Plots

![comm_65536](../Graphs/GraphsColumnSort/comm_65536.png)
![comm_262144](../Graphs/GraphsColumnSort/comm_262144.png)
![comm_1048576](../Graphs/GraphsColumnSort/comm_1048576.png)
![comm_4194304](../Graphs/GraphsColumnSort/comm_4194304.png)
![comm_16777216](../Graphs/GraphsColumnSort/comm_16777216.png)
![comm_67108864](../Graphs/GraphsColumnSort/comm_67108864.png)
![comm_268435456](../Graphs/GraphsColumnSort/comm_268435456.png)

### Strong Speedup/Weak Efficiency Plots

![comm_permuted_strong_speedup](../Graphs/GraphsColumnSort/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](../Graphs/GraphsColumnSort/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](../Graphs/GraphsColumnSort/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](../Graphs/GraphsColumnSort/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](../Graphs/GraphsColumnSort/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](../Graphs/GraphsColumnSort/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](../Graphs/GraphsColumnSort/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](../Graphs/GraphsColumnSort/comm_sorted_weak_efficiency.png)

