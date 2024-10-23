# Column Sort Analysis: Patralika Ghosh
My implementation of the column sort closely follows and is largely based on Leighton's Column Sort algorithm. The algorithm generates the array according to the input type, and each process is assigned a portion of indexes based on the input size and the number of processes. Each process then sorts its assigned column and returns the sorted result. This part of the code is parallelized however due to high communication overhead I was not able to parallelize the entire algorithm properly. Next, I transpose the matrix with the sorted columns and reshape it into submatrices of size (r/c) x r, where r is the number of rows and c is the number of columns. I repeat the process of column sorting and re-tranpose the matrix back to its original form. Afterward, I perform a shift in the matrix, where one half of the first column is filled with -inf values and the other half of the last column is filled with +inf values. I sort the columns again, then remove the -inf and +inf values, resulting in a fully column-sorted matrix.

Since, my implementation lacks sufficient parallelization, I am missing a couple of Caliper files because of network errors that I faced while trying to run my program for 1024 and 512 number of processors. My program is timing out for the largest array size for processors 32 and onwards and when it is run on number of processes for 512 and 1024. 

### Strong Scaling Plots
- Larger input sizes generally show better scalability, with higher speedups achieved as number of processes increase. This suggests that the algorithm can better utilize parallel resources when there is more data to process, which reduces the relative impact of overheads like communication and synchronization.

![main_65536](/ColumnSortGraphs/main_65536.png)
![main_262144](/ColumnSortGraphs/main_262144.png)
![main_1048576](/ColumnSortGraphs/main_1048576.png)
![main_4194304](/ColumnSortGraphs/main_4194304.png)
![main_16777216](/ColumnSortGraphs/main_16777216.png)
![main_67108864](/ColumnSortGraphs/main_67108864.png)
![main_268435456](/ColumnSortGraphs/main_268435456.png)

### Strong Speedup/Weak Efficiency Plots

![main_Perturbed_strong_speedup](/ColumnSortGraphs/main_Perturbed_strong_speedup.png)
![main_Perturbed_weak_efficiency](/ColumnSortGraphs/main_Perturbed_weak_efficiency.png)
![main_random_strong_speedup](/ColumnSortGraphs/main_random_strong_speedup.png)
![main_random_weak_efficiency](/ColumnSortGraphs/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/ColumnSortGraphs/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/ColumnSortGraphs/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/ColumnSortGraphs/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/ColumnSortGraphs/main_sorted_weak_efficiency.png)

### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- Unlike the graphs for main, these graphs generally decrease exponentially for every problem size (with some random variations that can be caused by outside factors, like grace speed).
- These graphs follow the same trend between input types; Random is the slowest by a large margin, followed by permuted, sorted, and reversed all with close execution types.
- These graphs all tend to approach 0 even for large problem sizes, demonstrating that for large computations, there is no bottleneck added as more processes are added. If we were to include comp_small in these, there would likely be a small bottleneck but not much. In addition, these graphs seem to decrease faster than the ones for main, which we will verify in the next section by analyzing the strong speedup/weak efficiency plots.
![comp_large_65536](/ColumnSortGraphs/comp_large_65536.png)
![comp_large_262144](/ColumnSortGraphs/comp_large_262144.png)
![comp_large_1048576](/ColumnSortGraphs/comp_large_1048576.png)
![comp_large_4194304](/ColumnSortGraphs/comp_large_4194304.png)
![comp_large_16777216](/ColumnSortGraphs/comp_large_16777216.png)
![comp_large_67108864](/ColumnSortGraphs/comp_large_67108864.png)
![comp_large_268435456](/ColumnSortGraphs/comp_large_268435456.png)

### Strong Speedup/Weak Efficiency Plots

![comp_large_permuted_strong_speedup](/ColumnSortGraphs/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/ColumnSortGraphs/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/ColumnSortGraphs/comp_large_random_strong_speedup.png)
![comp_large_random_weak_efficiency](/ColumnSortGraphs/comp_large_random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/ColumnSortGraphs/comp_large_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/ColumnSortGraphs/comp_large_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/ColumnSortGraphs/comp_large_sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/ColumnSortGraphs/comp_large_sorted_weak_efficiency.png)

### Strong Scaling Plots

![comm_65536](/ColumnSortGraphs/comm_65536.png)
![comm_262144](/ColumnSortGraphs/comm_262144.png)
![comm_1048576](/ColumnSortGraphs/comm_1048576.png)
![comm_4194304](/ColumnSortGraphs/comm_4194304.png)
![comm_16777216](/ColumnSortGraphs/comm_16777216.png)
![comm_67108864](/ColumnSortGraphs/comm_67108864.png)
![comm_268435456](/ColumnSortGraphs/comm_268435456.png)

### Strong Speedup/Weak Efficiency Plots

![comm_permuted_strong_speedup](/ColumnSortGraphs/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/ColumnSortGraphs/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/ColumnSortGraphs/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/ColumnSortGraphs/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/ColumnSortGraphs/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/ColumnSortGraphs/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/ColumnSortGraphs/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/ColumnSortGraphs/comm_sorted_weak_efficiency.png)

