# Radix Sort Analysis

## Overall Performance

Overall the performance of this algorthim is bad. The algorthim scales as expected but it simply takes way too long to run at higher array sizes.
Any further runs at higher sizes will require unavailable resources.
In order to further delve into why this algorthim is inefficient, data from 8 processors at a size of 1048576 will be used.
The entire process took an average time of 10380.546939 seconds per processor, or roughly 3 hours. Of this total time 10379.943732 of it was spent in the comm region, which is almost all of the time spent.
Furthermore the comm_large region takes up 10378.312745 seconds per processor on average, so the mast majority of the time is spent trasnferring data between processors after each local sort.
Inside this chunck both MPI_Recv (90.510783 seconds per processor on average), and MPI_Send (50.200803 seconds per processor on average) are called twice.
However sense these functions require a response in order to proceed, each of them is called sequentially, which massively hurts performance.
In the future this program will be further optimized.
![main_65536](https://github.com/user-attachments/assets/28d30780-7e09-437c-b79e-6ca9d68c00bf)
![main_262144](https://github.com/user-attachments/assets/0b8c3251-b001-4600-99ec-6631dfb4768f)
![main_1048576](https://github.com/user-attachments/assets/9a8bb6bc-2eb8-4a25-95f6-2f8d31c99531)
![main_4194304](https://github.com/user-attachments/assets/04f881fd-673c-4365-8d45-d4c280b119bb)

## Speedup and Efficiency

This graphs speedup and efficiency trends are as expected. Since radix sort is a bitwise sort, there is no difference between any of the different element types, they all take exactly the same amount of time. Speedup increases linearlly with the number of processors, which means that this program is parallelized correctly.
Efficiency seems to roughly decrease with the number of processors, which is expected as a higher number of processors will result in lower efficiency.
![comm_permuted_strong_speedup](https://github.com/user-attachments/assets/6983ab83-ce3a-424e-b0cc-615f3823b69f)
![comm_permuted_weak_efficiency](https://github.com/user-attachments/assets/5e482ff4-dfc3-4af6-af2c-55be7837dad4)
![comm_random_strong_speedup](https://github.com/user-attachments/assets/211299ed-7cff-41b9-a723-9e1f5b0476fc)
![comm_random_weak_efficiency](https://github.com/user-attachments/assets/34bd3662-c8d6-4036-8f81-f1c6f16caf13)
![comm_reversed_strong_speedup](https://github.com/user-attachments/assets/bead06bb-b6f5-41ea-b072-ea25b916ba62)
![comm_reversed_weak_efficiency](https://github.com/user-attachments/assets/b90a778b-6bf7-4af1-a81c-a074f4307a47)
![comm_sorted_strong_speedup](https://github.com/user-attachments/assets/24c01562-4d76-4bc6-97b5-41f059724674)
![comm_sorted_weak_efficiency](https://github.com/user-attachments/assets/d5ac9de1-dca1-4b35-896d-d4ae2d766d29)

There is little difference between speedups in the comp and coom regions, as increasing the number of processors will cause an increase in speedup, as these array sizes are all big enough to benifit from that speedup.

![comp_large_permuted_strong_speedup](https://github.com/user-attachments/assets/8bdf0552-6c10-45b7-b35f-e193d8a76ee6)
![comp_large_permuted_weak_efficiency](https://github.com/user-attachments/assets/08ab8c3f-5067-4c17-b99d-031f8dca1867)
![comp_large_random_strong_speedup](https://github.com/user-attachments/assets/840e460b-c9f6-4bd2-972a-6558244b00d7)
![comp_large_random_weak_efficiency](https://github.com/user-attachments/assets/695288c9-17f6-49a0-965a-64d2c8d779b3)
![comp_large_reversed_strong_speedup](https://github.com/user-attachments/assets/0b15795e-3993-4902-84d1-2c97c99be6d6)
![comp_large_reversed_weak_efficiency](https://github.com/user-attachments/assets/d85bb5fb-eb5d-481d-b10a-92dc16f66e3f)
![comp_large_sorted_strong_speedup](https://github.com/user-attachments/assets/059fe96e-81b9-47cb-9b37-73b9f418a149)
![comp_large_sorted_weak_efficiency](https://github.com/user-attachments/assets/ad5f251d-2b28-4f69-9411-117b57be7782)

There is minialy difference between the main graphs and the comm graphs, espically because the comm chunck takes up over 99% of the total runtime.

![main_permuted_strong_speedup](https://github.com/user-attachments/assets/76a3be21-525a-492b-a2e0-57354bcfcbf1)
![main_permuted_weak_efficiency](https://github.com/user-attachments/assets/c143b532-552f-4166-a532-0b5b6c3173b5)
![main_random_strong_speedup](https://github.com/user-attachments/assets/a1b270ae-b095-4153-ac21-dfb05821d2b2)
![main_random_weak_efficiency](https://github.com/user-attachments/assets/e85cd398-dd8a-47e0-a117-c080cbb05862)
![main_reversed_strong_speedup](https://github.com/user-attachments/assets/063b8391-3db6-486c-bad6-7f4f983620a7)
![main_reversed_weak_efficiency](https://github.com/user-attachments/assets/719f5df3-1ce9-4aff-83a0-50f77fd2cf5a)
![main_sorted_strong_speedup](https://github.com/user-attachments/assets/f124e947-70fd-4e8e-897b-142a4344863f)
![main_sorted_weak_efficiency](https://github.com/user-attachments/assets/bd7dfc91-570e-41d2-ad96-031161e9f968)

