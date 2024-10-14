# Merge Sort Analysis: Ariela Mitrani
This document will focus on the analysis of the parallelized merge sort algorithm that was implemented for this project using a variety of different visualizations generated from Caliper files. The report file specified the following graphs to analyze for the presentation, so the graphs I will be analyzing in this document are:

For each of comp_large, comm, and main:
- Strong scaling plots for each input_size with lines for input_type (7 plots - 4 lines each)
- Strong scaling speedup plot for each input_type (4 plots)
- Weak scaling plots for each input_type (4 plots)

Only a subset of these will be in our final presentation, but the below will include a detailed analysis on each of these groups. These were tested with array sizes 2^16, 2^18, 2^20, 2^22, 2^24, 2^26, 2^28, process numbers 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, and input types sorted, random, reverse sorted, and 1% perturbed.

## Main: Total Time for Program Execution
For the measurements for this section, we used Max time/rank from the Cali file, which would be the time taken by the task that does the final merge and the correctness check.

### Strong Scaling Plots
This section has 7 graphs that span the seven different array sizes specified above, with each one having a line for each input type. the most important observations from these graphs are the following:

- The smallest array size, 65536, shows that the total program time actually increases as more processes are added. This is because the added communication overhead for additional processes heavily outweighs any benefit to computation time that those processes add on such a small problem size.
- As the array sizes increase, the point where total execution time gets worse instead of better with more processes shifts to the right (higher number of processes), and the process numbers before that point have a much steeper downward trend. This is because as the problem size increases, the value of adding processes also increases, and this break-even point is larger for larger problem sizes.
- The difference between the different input types is minimal, but the slowest is consistently random. This makes sense, as the program is not able to optimize the likely next choice as efficiently. Permuted was typically the next slowest, followed by sorted and finally reversed. The important thing to note here is that data that was random was consistently slower, as the other three times were much closer to each other than to the random time.
- As the problem size got large enough to make all the added processes helpful, the graph formed a smooth exponential decrease. We will discuss the slope of this in the following section. Another thing of note here is that the minimum time for the largest problem size (268435456) never dips below 5 seconds, suggesting that this is the amount of time that cannot be parallelized effectively.

![main_65536](/MergeSortGraphs/main_65536.png)
![main_262144](/MergeSortGraphs/main_262144.png)
![main_1048576](/MergeSortGraphs/main_1048576.png)
![main_4194304](/MergeSortGraphs/main_4194304.png)
![main_16777216](/MergeSortGraphs/main_16777216.png)
![main_67108864](/MergeSortGraphs/main_67108864.png)
![main_268435456](/MergeSortGraphs/main_268435456.png)

### Strong Speedup/Weak Efficiency Plots
These graphs show both the strong speedup and weak efficiency relative to time on two processes for all of the given input types. Each graph has a line for every input size. The observations we can make are as follows:

- There isn't a significant difference between these graphs for permuted, random, reversed, and sorted data, because they all follow the same trends even if the times differ slightly.
- The strong speedup for these graphs seems to be much closer to a linear speedup at the beginning, before leveling out to a constant or even decreasing speedup with processes added. Once again, this is because the problem size is not large enough to have a significant speedup after a certain number of processes, and the amount it speeds up starts to decrease after that point.
- Following the same reasoning, the larger problem sizes are always higher on the graph (e.g. the lines are in order of problem size). This is because for a larger problem size, the amount of speedup that is obtained by using more processors tends to be better, and that speedup doesn't hit a limit until a higher processor number (and the limit is higher).
- Looking at the weak efficiency graphs, we once again see the trend that larger problem sizes have a higher efficiency overall. However, all of the weak scaling for these processes decreases quickly until it levels out around the point where it cannot decrease any more. This relates to the slope we saw in the strong scaling plots; Even though the problem execution time decreased on a curve as the process number increased, this was not a proportional relationship (where doubling the processes halves the execution time), and thus the weak scaling efficiency drops away from 1 quickly.
![main_permuted_strong_speedup](/MergeSortGraphs/main_permuted_strong_speedup.png)
![main_permuted_weak_efficiency](/MergeSortGraphs/main_permuted_weak_efficiency.png)
![main_random_strong_speedup](/MergeSortGraphs/main_random_strong_speedup.png)
![main_random_weak_efficiency](/MergeSortGraphs/main_random_weak_efficiency.png)
![main_reversed_strong_speedup](/MergeSortGraphs/main_reversed_strong_speedup.png)
![main_reversed_weak_efficiency](/MergeSortGraphs/main_reversed_weak_efficiency.png)
![main_sorted_strong_speedup](/MergeSortGraphs/main_sorted_strong_speedup.png)
![main_sorted_weak_efficiency](/MergeSortGraphs/main_sorted_weak_efficiency.png)

## Comp_Large: Average Time Spent Computing (Sorting) Per Processor
For the measurements for this section, we used Avg time/rank from the Cali file, which would be the average amount of time each task takes to sort and merge its sections of the array.

### Strong Scaling Plots
These graphs are set up the same way as the strong scaling graphs for main, with the following observations:

- Unlike the graphs for main, these graphs generally decrease exponentially for every problem size (with some random variations that can be caused by outside factors, like grace speed).
- These graphs follow the same trend between input types; Random is the slowest by a large margin, followed by permuted, sorted, and reversed all with close execution types.
- These graphs all tend to approach 0 even for large problem sizes, demonstrating that for large computations, there is no bottleneck added as more processes are added. If we were to include comp_small in these, there would likely be a small bottleneck but not much. In addition, these graphs seem to decrease faster than the ones for main, which we will verify in the next section by analyzing the strong speedup/weak efficiency plots.
![comp_large_65536](/MergeSortGraphs/comp_large_65536.png)
![comp_large_262144](/MergeSortGraphs/comp_large_262144.png)
![comp_large_1048576](/MergeSortGraphs/comp_large_1048576.png)
![comp_large_4194304](/MergeSortGraphs/comp_large_4194304.png)
![comp_large_16777216](/MergeSortGraphs/comp_large_16777216.png)
![comp_large_67108864](/MergeSortGraphs/comp_large_67108864.png)
![comp_large_268435456](/MergeSortGraphs/comp_large_268435456.png)

### Strong Speedup/Weak Efficiency Plots
These graphs show strong speedup and weak efficiency for the large computations in our program (such as one-processor merge sort and merging sorted arrays). We can observe the following:

- There is a strange bump on the weak efficiency graphs for 256 processors. This may be for a number of reasons, but I ran this test at a different time than some of the others so it may just have been network conditions.
- The weak efficiency is pretty consistent and close to 1, but starts to decrease slightly as the process count gets large. There is also a sharp (but small) decrease for the first few processor numbers, which may just be because the additional computations to merge/
![comp_large_permuted_strong_speedup](/MergeSortGraphs/comp_large_permuted_strong_speedup.png)
![comp_large_permuted_weak_efficiency](/MergeSortGraphs/comp_large_permuted_weak_efficiency.png)
![comp_large_random_strong_speedup](/MergeSortGraphs/comp_large_random_strong_speedup.png)
![comp_large_random_weak_efficiency](/MergeSortGraphs/comp_large_random_weak_efficiency.png)
![comp_large_reversed_strong_speedup](/MergeSortGraphs/comp_large_reversed_strong_speedup.png)
![comp_large_reversed_weak_efficiency](/MergeSortGraphs/comp_large_reversed_weak_efficiency.png)
![comp_large_sorted_strong_speedup](/MergeSortGraphs/comp_large_sorted_strong_speedup.png)
![comp_large_sorted_weak_efficiency](/MergeSortGraphs/comp_large_sorted_weak_efficiency.png)


## Comm: Average Time Spent Communicating Per Processor
### Strong Scaling Plots
![comm_65536](/MergeSortGraphs/comm_65536.png)
![comm_262144](/MergeSortGraphs/comm_262144.png)
![comm_1048576](/MergeSortGraphs/comm_1048576.png)
![comm_4194304](/MergeSortGraphs/comm_4194304.png)
![comm_16777216](/MergeSortGraphs/comm_16777216.png)
![comm_67108864](/MergeSortGraphs/comm_67108864.png)
![comm_268435456](/MergeSortGraphs/comm_268435456.png)
### Strong Speedup/Weak Efficiency Plots
![comm_permuted_strong_speedup](/MergeSortGraphs/comm_permuted_strong_speedup.png)
![comm_permuted_weak_efficiency](/MergeSortGraphs/comm_permuted_weak_efficiency.png)
![comm_random_strong_speedup](/MergeSortGraphs/comm_random_strong_speedup.png)
![comm_random_weak_efficiency](/MergeSortGraphs/comm_random_weak_efficiency.png)
![comm_reversed_strong_speedup](/MergeSortGraphs/comm_reversed_strong_speedup.png)
![comm_reversed_weak_efficiency](/MergeSortGraphs/comm_reversed_weak_efficiency.png)
![comm_sorted_strong_speedup](/MergeSortGraphs/comm_sorted_strong_speedup.png)
![comm_sorted_weak_efficiency](/MergeSortGraphs/comm_sorted_weak_efficiency.png)
