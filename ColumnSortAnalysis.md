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

![main_permuted_strong_speedup](/ColumnSortGraphs/main_Perturbed_strong_speedup.png)
![main_permuted_weak_efficiency](/ColumnSortGraphs/main_Perturbed_weak_efficiency.png)
![main_random_strong_speedup](/ColumnSortGraphs/main_random_strong_speedup.png)
![main_random_weak_efficiency](/ColumnSortGraphs/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/ColumnSortGraphs/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/ColumnSortGraphs/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/ColumnSortGraphs/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/ColumnSortGraphs/main_sorted_weak_efficiency.png)

## Comp_Large: Average Time Spent Computing (Sorting) Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task takes to sort and merge its sections of the array.

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
These graphs show strong speedup and weak efficiency for the large computations in our program (such as one-processor merge sort and merging sorted arrays). We can observe the following:

- There is a strange bump on the weak efficiency graphs for 256 processors. This may be for a number of reasons, but I ran this test at a different time than some of the others so it may just have been network conditions, or it may be that the architecture distributes the processes differently for 256 processes (across nodes/tasks).
- The weak efficiency is pretty consistent and close to 1, but starts to decrease slightly as the process count gets large. This shows that at least for our computation steps, this program is pretty well parallelized (e.g. the amount of time to double the problem size with double the processors remains about constant).
- The strong speedup is close to linear, but the smaller the problem size, the more it starts to deviate from that linearity. This is  because, past a certain point, the amount of work each task does becomes so small that the added processors don't add as much efficiency. This is also why we see a slight decrease in weak efficiency with an increasing process count.
- Once again, there are no huge differences between the types of inputs for efficiency. In addition, just like for main, the larger problem sizes tend to hold the desired trends (linearity and weak efficiency = 1) better, as demonstrated by the lines being roughly in order of problem size.
![comp_large_permuted_strong_speedup](/ColumnSortGraphs/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/ColumnSortGraphs/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/ColumnSortGraphs/comp_large_random_strong_speedup.png)
![comp_large_random_weak_efficiency](/ColumnSortGraphs/comp_large_random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/ColumnSortGraphs/comp_large_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/ColumnSortGraphs/comp_large_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/ColumnSortGraphs/comp_large_sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/ColumnSortGraphs/comp_large_sorted_weak_efficiency.png)


## Comm: Average Time Spent Communicating Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task spends sending or receiving data from other tasks.

### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- For smaller problem sizes, the average time spent communicating increases as process count increases. However, like our graphs for main, the graphs start to show the expected trend with larger process sizes, which is a decrease in average communication time as process count increases. This seems counter-intuitive, as with more processes we would expect more communication. However, even though we do have more communication as more tasks are added, we end up having less communication time per processor. This is because the smaller operations that are present for more processes are not time-consuming enough to lead to the same amount of average work for the processes.
- A strange feature of these graphs is the sharp increase from 2 to 4 processes. This is due to the structure of the program, which makes the communication between the last two processes more ideal than other communications. Because of this, the comm time for 2 processes is lower than that for 4+ processes, and thus it deviates from the trend. If I were to change my implementation so that this wasn't the case, this would be a smooth curve all the way through -- however, the original design I had and the changes I made do not have the most optimal communication.
- Unlike main and comp, there is no relationship between input type and these graphs, which makes sense because the communication time does not consider the contents of the data it is sending, but rather just the data itself.
![comm_65536](/ColumnSortGraphs/comm_65536.png)
![comm_262144](/ColumnSortGraphs/comm_262144.png)
![comm_1048576](/ColumnSortGraphs/comm_1048576.png)
![comm_4194304](/ColumnSortGraphs/comm_4194304.png)
![comm_16777216](/ColumnSortGraphs/comm_16777216.png)
![comm_67108864](/ColumnSortGraphs/comm_67108864.png)
![comm_268435456](/ColumnSortGraphs/comm_268435456.png)

### Strong Speedup/Weak Efficiency Plots
These graphs show strong speedup and weak efficiency for the communication between tasks in our program (such as send and receive). We can observe the following:

- Like the other strong speedup graphs, the larger the problem, the better the speedup. However, these speedup graphs are nowhere near linear, and actually seem to be pretty constant/decrease to reach zero. This is because the communication, while it takes up slightly less time on average for more processes, definitely doesn't decrease efficiently for large sizes of N (and isn't meant to). The implementation of this algorithm focused mostly on achieving strong linear speedup for the computation portion (which is the expensive part of this problem), rather than minimizing communication time.
- In the weak efficiency graphs, we see that these rapidly go to zero. Again, this is because the communication time isn't able to parallelize well for a merge sort algorithm - this is the feature that is bottlenecking the speedup of main. However, we can still observe that larger problem sizes have slightly larger values for efficiency, although not by much. This is also reflected in the strong speedup graphs, where larger problems exhibited better speedup.
![comm_permuted_strong_speedup](/ColumnSortGraphs/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/ColumnSortGraphs/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/ColumnSortGraphs/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/ColumnSortGraphs/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/ColumnSortGraphs/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/ColumnSortGraphs/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/ColumnSortGraphs/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/ColumnSortGraphs/comm_sorted_weak_efficiency.png)


## Possible Optimizations Missed
After running all of my analysis and looking at my code structure, I determined a few optimizations that could be done to make my code more efficient. When I discussed with the professor, she said that while I didn't have to rerun my code/regenerate my graphs, I should discuss some of these optimizations in this analysis to see how I could improve my algorithm for future work. The changes I would make to optimize this are the following:

- Generate a universally sorted array (all of the data on P0 is less than all of the data on P1, etc), and implement an in-order merge that takes two sorted arrays where all of one is strictly greater than all of the other. This would make the merge step much faster if we add a condition that one array in the merge is always strictly less than the other because we would only have one comparison to do.
- To make the above optimization possible, we would also have to send the data to the same task(s) as long as possible. My current implementation sends data from two different nodes to a third node, but a more efficient way to do this would be to have the merge happen on one of the nodes with data, reducing the amount of data that needs to be sent per merge step by half. To do this, process A would merge itself with process B rather than process C merging process B and process A together.

However, aside from these optimizations, my merge sort is fairly efficient, even in the computation step which exhibits almost strong scaling. Merge sort isn't a perfectly parallelizable algorithm due to the communication overhead, but this implementation exhibits/explains the required trends.
