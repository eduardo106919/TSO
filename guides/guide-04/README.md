# Guide 04 - Benchmarking

This guide will allow you to experiment with different benchmarking tools. Namely, we will try out a generic OS benchmarking suite that can stress test CPU, RAM, I/O, process-based operations, among others. The other tool is targeted specifically at assessing the performance of block and file-based I/O. We will combine the benchmark outputs with observability tools to better understand the observed results. Finally, we will measure the performance overhead of tracing tools.

**Tools/Technologies**: We will use `stress-ng` as our generic OS benchmark, `fio` as our storage benchmark, and `htop` as the monitoring tool. `strace` and our eBPF-based tracer, done in the last guide, will be assessed, performance-wise, with `fio`.

**Learning Outcomes**: Gain practical experience with performance evaluation. Experiment with different benchmarks and sets of workloads. Analyse the output of benchmarking and monitoring tools. Understand the performance overhead of tracing.

## Benchmarking the CPU and RAM

### Using stress-ng to exercise multiple resource

#### CPU stressor

Input: `stress-ng --cpu 0 --timeout 1m --metrics --times`

Output:

```
stress-ng: info:  [3098] setting to a 1 min run per stressor
stress-ng: info:  [3098] dispatching hogs: 4 cpu
stress-ng: metrc: [3098] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [3098]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [3098] cpu              347678     60.00    238.75      0.55      5794.38        1452.91        99.70          8136
stress-ng: info:  [3098] for a 60.01s run time:
stress-ng: info:  [3098]     240.04s available CPU time
stress-ng: info:  [3098]     238.74s user time   ( 99.46%)
stress-ng: info:  [3098]       0.55s system time (  0.23%)
stress-ng: info:  [3098]     239.29s total time  ( 99.69%)
stress-ng: info:  [3098] load average: 3.29 1.40 0.54
stress-ng: info:  [3098] skipped: 0
stress-ng: info:  [3098] passed: 4: cpu (4)
stress-ng: info:  [3098] failed: 0
stress-ng: info:  [3098] metrics untrustworthy: 0
stress-ng: info:  [3098] successful run completed in 1 min
```

Input: `stress-ng --cpu 2 --timeout 1m --metrics --times`

Output:

```
stress-ng: info:  [3130] setting to a 1 min run per stressor
stress-ng: info:  [3130] dispatching hogs: 2 cpu
stress-ng: metrc: [3130] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [3130]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [3130] cpu              199076     60.00    118.91      0.67      3317.91        1664.80        99.65          8328
stress-ng: info:  [3130] for a 60.00s run time:
stress-ng: info:  [3130]     240.02s available CPU time
stress-ng: info:  [3130]     118.91s user time   ( 49.54%)
stress-ng: info:  [3130]       0.66s system time (  0.27%)
stress-ng: info:  [3130]     119.57s total time  ( 49.82%)
stress-ng: info:  [3130] load average: 1.59 1.27 0.59
stress-ng: info:  [3130] skipped: 0
stress-ng: info:  [3130] passed: 2: cpu (2)
stress-ng: info:  [3130] failed: 0
stress-ng: info:  [3130] metrics untrustworthy: 0
stress-ng: info:  [3130] successful run completed in 1 min
```

**Analysis**: CPU utilization measures the percentage of time the processor spends executing non-idle tasks, reflecting immediate computational intensity. In contrast, the load average represents the number of processes in the system run queue that are either currently executing or waiting for CPU time. During the first run, the load average of 3.29 indicated that nearly all four available logical cores were occupied. The CPU utilization reached 99.69%, signaling total resource saturation with negligible idle cycles. The second run halved this demand, resulting in a load average of 1.59 and roughly 50% utilization. These metrics confirm that the scheduler efficiently distributed the synthetic "hogs" across the available hardware threads.

#### VM Stressor

Input: `stress-ng --vm 2 --vm-bytes 2G --timeout 1m --metrics --times`

Output:

```
stress-ng: info:  [3707] setting to a 1 min run per stressor
stress-ng: info:  [3707] dispatching hogs: 2 vm
stress-ng: metrc: [3707] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [3707]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [3707] vm             11001941     60.02     95.20     24.63    183314.94       91816.38        99.83       1183112
stress-ng: info:  [3707] for a 60.03s run time:
stress-ng: info:  [3707]     240.13s available CPU time
stress-ng: info:  [3707]      95.19s user time   ( 39.64%)
stress-ng: info:  [3707]      24.62s system time ( 10.25%)
stress-ng: info:  [3707]     119.81s total time  ( 49.89%)
stress-ng: info:  [3707] load average: 1.04 0.38 0.31
stress-ng: info:  [3707] skipped: 0
stress-ng: info:  [3707] passed: 2: vm (2)
stress-ng: info:  [3707] failed: 0
stress-ng: info:  [3707] metrics untrustworthy: 0
stress-ng: info:  [3707] successful run completed in 1 min
```

Input: `stress-ng --vm 4 --vm-bytes 2G --timeout 1m --metrics --times`

Output:

```
stress-ng: info:  [3725] setting to a 1 min run per stressor
stress-ng: info:  [3725] dispatching hogs: 4 vm
stress-ng: metrc: [3725] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [3725]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [3725] vm             11572940     60.00    201.44     37.65    192873.57       48403.09        99.62        593564
stress-ng: info:  [3725] for a 60.01s run time:
stress-ng: info:  [3725]     240.03s available CPU time
stress-ng: info:  [3725]     201.44s user time   ( 83.92%)
stress-ng: info:  [3725]      37.65s system time ( 15.69%)
stress-ng: info:  [3725]     239.09s total time  ( 99.61%)
stress-ng: info:  [3725] load average: 2.74 1.00 0.53
stress-ng: info:  [3725] skipped: 0
stress-ng: info:  [3725] passed: 4: vm (4)
stress-ng: info:  [3725] failed: 0
stress-ng: info:  [3725] metrics untrustworthy: 0
stress-ng: info:  [3725] successful run completed in 1 min
```

**Analysis**: Virtual memory utilization metrics reveal a transition from efficient data throughput to significant memory bandwidth saturation and kernel overhead. While the initial two-worker configuration achieved high per-instance efficiency, doubling the stressors to four workers yielded only a marginal 5% increase in total bogo operations. This bottleneck is evidenced by the system time escalating to 15.69%, reflecting the kernel's increased burden in managing page table entries and memory mapping. Furthermore, the sharp reduction in RSS Max per worker suggests aggressive resource contention, where physical RAM limitations force the operating system to throttle individual process footprints. These results indicate that while the CPU has the logical capacity for four threads, the underlying memory subsystem reaches peak performance with only two active high-bandwidth stressors.

#### Fork Stressor

Input: `stress-ng --fork 8 --timeout 1m --metrics --times`

Output:

```
stress-ng: info:  [3773] setting to a 1 min run per stressor
stress-ng: info:  [3773] dispatching hogs: 8 fork
stress-ng: metrc: [3773] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [3773]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [3773] fork             428511     60.00     90.49    144.16      7141.77        1826.22        48.88          3580
stress-ng: info:  [3773] for a 60.01s run time:
stress-ng: info:  [3773]     240.03s available CPU time
stress-ng: info:  [3773]      90.48s user time   ( 37.70%)
stress-ng: info:  [3773]     144.16s system time ( 60.06%)
stress-ng: info:  [3773]     234.64s total time  ( 97.75%)
stress-ng: info:  [3773] load average: 7.13 2.41 1.08
stress-ng: info:  [3773] skipped: 0
stress-ng: info:  [3773] passed: 8: fork (8)
stress-ng: info:  [3773] failed: 0
stress-ng: info:  [3773] metrics untrustworthy: 0
stress-ng: info:  [3773] successful run completed in 1 min
```

Input: `stress-ng --fork 16 --timeout 1m --metrics --times`

Output:

```
stress-ng: info:  [432302] setting to a 1 min run per stressor
stress-ng: info:  [432302] dispatching hogs: 16 fork
stress-ng: metrc: [432302] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [432302]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [432302] fork             476943     60.00     96.88    139.74      7948.93        2015.67        24.65          3620
stress-ng: info:  [432302] for a 60.02s run time:
stress-ng: info:  [432302]     240.07s available CPU time
stress-ng: info:  [432302]      96.87s user time   ( 40.35%)
stress-ng: info:  [432302]     139.74s system time ( 58.21%)
stress-ng: info:  [432302]     236.61s total time  ( 98.56%)
stress-ng: info:  [432302] load average: 14.54 5.45 2.22
stress-ng: info:  [432302] skipped: 0
stress-ng: info:  [432302] passed: 16: fork (16)
stress-ng: info:  [432302] failed: 0
stress-ng: info:  [432302] metrics untrustworthy: 0
stress-ng: info:  [432302] successful run completed in 1 min
```

**Analysis**: The transition from computational stressors to the fork stressor shifts the system's primary bottleneck from raw processing power to the kernel’s process management subsystem. Because the fork operation requires the operating system to constantly create, track, and terminate process identifiers and memory structures, the system time dominates the CPU profile, peaking at roughly 60% of total available time. In the 8-worker run, the load average of 7.13 already exceeded your 4-core capacity, resulting in significant context switching as the scheduler struggled to balance the rapid process churn. When increased to 16 workers, the load average spiked to 14.54, but total bogo ops/s improved only slightly, confirming that the kernel's process table and scheduler were effectively saturated. With `htop` we can see that the CPU was not doing "work" for the user, but was instead "spinning" in kernel mode to handle the overhead of managing a massive, competing queue of tasks.

#### MQ Stressor

Input: `stress-ng --mq 0 -t 30s --timeout 1m --metrics --times --perf`

Output:

```
stress-ng: info:  [909303] setting to a 1 min run per stressor
stress-ng: info:  [909303] dispatching hogs: 4 mq
stress-ng: metrc: [909303] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [909303]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [909303] mq             49926506     60.00     37.06    188.47    832082.81      221370.23        93.97          3296
stress-ng: info:  [909303] mq:
stress-ng: info:  [909303]            219,907,537,768 CPU Clock                      3.664 B/sec
stress-ng: info:  [909303]            222,442,694,096 Task Clock                     3.707 B/sec
stress-ng: info:  [909303]                        268 Page Faults Total              4.466 /sec
stress-ng: info:  [909303]                        264 Page Faults Minor              4.399 /sec
stress-ng: info:  [909303]                          4 Page Faults Major              0.067 /sec
stress-ng: info:  [909303]                 43,983,692 Context Switches               0.733 M/sec
stress-ng: info:  [909303]                    669,064 Cgroup Switches               11.149 K/sec
stress-ng: info:  [909303]                        672 CPU Migrations                11.197 /sec
stress-ng: info:  [909303]                          0 Alignment Faults               0.000 /sec
stress-ng: info:  [909303]                          0 Emulation Faults               0.000 /sec
stress-ng: info:  [909303] for a 60.01s run time:
stress-ng: info:  [909303]     240.05s available CPU time
stress-ng: info:  [909303]      37.06s user time   ( 15.44%)
stress-ng: info:  [909303]     188.47s system time ( 78.51%)
stress-ng: info:  [909303]     225.53s total time  ( 93.95%)
stress-ng: info:  [909303] load average: 4.26 3.20 2.01
stress-ng: info:  [909303] skipped: 0
stress-ng: info:  [909303] passed: 4: mq (4)
stress-ng: info:  [909303] failed: 0
stress-ng: info:  [909303] metrics untrustworthy: 0
stress-ng: info:  [909303] successful run completed in 1 min
```

Input: `stress-ng --mq 2 -t 30s --timeout 1m --metrics --times --perf`

Output:

```
stress-ng: info:  [918874] setting to a 1 min run per stressor
stress-ng: info:  [918874] dispatching hogs: 2 mq
stress-ng: metrc: [918874] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [918874]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [918874] mq             47884503     60.00     30.02    145.88    798052.80      272226.70       146.58          3436
stress-ng: info:  [918874] mq:
stress-ng: info:  [918874]            161,378,615,626 CPU Clock                      2.689 B/sec
stress-ng: info:  [918874]            166,062,795,682 Task Clock                     2.767 B/sec
stress-ng: info:  [918874]                        146 Page Faults Total              2.433 /sec
stress-ng: info:  [918874]                        146 Page Faults Minor              2.433 /sec
stress-ng: info:  [918874]                          0 Page Faults Major              0.000 /sec
stress-ng: info:  [918874]                  5,588,204 Context Switches              93.110 K/sec
stress-ng: info:  [918874]                  5,175,502 Cgroup Switches               86.233 K/sec
stress-ng: info:  [918874]                        152 CPU Migrations                 2.533 /sec
stress-ng: info:  [918874]                          0 Alignment Faults               0.000 /sec
stress-ng: info:  [918874]                          0 Emulation Faults               0.000 /sec
stress-ng: info:  [918874] for a 60.02s run time:
stress-ng: info:  [918874]     240.07s available CPU time
stress-ng: info:  [918874]      30.02s user time   ( 12.50%)
stress-ng: info:  [918874]     145.87s system time ( 60.76%)
stress-ng: info:  [918874]     175.89s total time  ( 73.27%)
stress-ng: info:  [918874] load average: 2.98 2.96 2.03
stress-ng: info:  [918874] skipped: 0
stress-ng: info:  [918874] passed: 2: mq (2)
stress-ng: info:  [918874] failed: 0
stress-ng: info:  [918874] metrics untrustworthy: 0
stress-ng: info:  [918874] successful run completed in 1 min
```

**Analysis**: Testing the POSIX message queue stressor highlights a massive shift toward kernel-level Inter-Process Communication overhead, where the system spends the vast majority of its cycles managing message buffers rather than executing user code. In the first run with four workers, the system time hit a staggering 78.51%, and the `perf` metrics revealed a crushing 43.9 million context switches, indicating that the CPU is constantly swapping between tasks to handle the rapid-fire message passing. Dropping to two workers significantly improved the efficiency—jumping from 221k to 272k bogo ops/s per unit of CPU time—because the reduction in active stressors dramatically lowered the frequency of context switches from 733k/sec to just 93k/sec. While the load average of 4.26 in the first run fully saturated the four cores, the performance data proves that the real bottleneck isn't the core count, but the high latency of kernel-space transitions and synchronization required for IPC.

#### Mixing it all

Input: `stress-ng --cpu 4 --vm 2 --fork 8 --mq 3 --timeout 1m --metrics --times --perf`

Output:

```
stress-ng: info:  [927626] setting to a 1 min run per stressor
stress-ng: info:  [927626] dispatching hogs: 4 cpu, 2 vm, 8 fork, 3 mq
stress-ng: metrc: [927626] stressor       bogo ops real time  usr time  sys time   bogo ops/s     bogo ops/s CPU used per       RSS Max
stress-ng: metrc: [927626]                           (secs)    (secs)    (secs)   (real time) (usr+sys time) instance (%)          (KB)
stress-ng: metrc: [927626] cpu               87867     60.00     59.54      5.14      1464.34        1358.39        26.95          8220
stress-ng: metrc: [927626] vm              1447771     60.01     25.69      7.37     24124.11       43796.54        27.54        150836
stress-ng: metrc: [927626] fork             206795     60.00     41.56     61.64      3446.43        2003.88        21.50          3576
stress-ng: metrc: [927626] mq             10551132     60.00      5.30     30.99    175840.61      290757.72        20.16          3244
stress-ng: info:  [927626] cpu:
stress-ng: info:  [927626]             62,635,060,008 CPU Clock                      1.043 B/sec
stress-ng: info:  [927626]             62,802,368,680 Task Clock                     1.046 B/sec
stress-ng: info:  [927626]                      2,068 Page Faults Total             34.431 /sec
stress-ng: info:  [927626]                      2,068 Page Faults Minor             34.431 /sec
stress-ng: info:  [927626]                          0 Page Faults Major              0.000 /sec
stress-ng: info:  [927626]                     94,108 Context Switches               1.567 K/sec
stress-ng: info:  [927626]                     10,692 Cgroup Switches              178.017 /sec
stress-ng: info:  [927626]                         56 CPU Migrations                 0.932 /sec
stress-ng: info:  [927626]                          0 Alignment Faults               0.000 /sec
stress-ng: info:  [927626]                          0 Emulation Faults               0.000 /sec
stress-ng: info:  [927626] vm:
stress-ng: info:  [927626]             32,950,334,558 CPU Clock                      0.549 B/sec
stress-ng: info:  [927626]             33,071,552,106 Task Clock                     0.551 B/sec
stress-ng: info:  [927626]                  1,912,518 Page Faults Total             31.842 K/sec
stress-ng: info:  [927626]                  1,912,518 Page Faults Minor             31.842 K/sec
stress-ng: info:  [927626]                          0 Page Faults Major              0.000 /sec
stress-ng: info:  [927626]                     70,494 Context Switches               1.174 K/sec
stress-ng: info:  [927626]                      5,578 Cgroup Switches               92.871 /sec
stress-ng: info:  [927626]                         40 CPU Migrations                 0.666 /sec
stress-ng: info:  [927626]                          0 Alignment Faults               0.000 /sec
stress-ng: info:  [927626]                          0 Emulation Faults               0.000 /sec
stress-ng: info:  [927626] fork:
stress-ng: info:  [927626]             64,682,403,104 CPU Clock                      1.077 B/sec
stress-ng: info:  [927626]             65,677,012,672 Task Clock                     1.093 B/sec
stress-ng: info:  [927626]                 10,145,496 Page Faults Total              0.169 M/sec
stress-ng: info:  [927626]                 10,145,496 Page Faults Minor              0.169 M/sec
stress-ng: info:  [927626]                          0 Page Faults Major              0.000 /sec
stress-ng: info:  [927626]                    494,936 Context Switches               8.240 K/sec
stress-ng: info:  [927626]                     12,136 Cgroup Switches              202.059 /sec
stress-ng: info:  [927626]                        776 CPU Migrations                12.920 /sec
stress-ng: info:  [927626]                          0 Alignment Faults               0.000 /sec
stress-ng: info:  [927626]                          0 Emulation Faults               0.000 /sec
stress-ng: info:  [927626] mq:
stress-ng: info:  [927626]             33,929,470,980 CPU Clock                      0.565 B/sec
stress-ng: info:  [927626]             34,954,791,549 Task Clock                     0.582 B/sec
stress-ng: info:  [927626]                        207 Page Faults Total              3.446 /sec
stress-ng: info:  [927626]                        207 Page Faults Minor              3.446 /sec
stress-ng: info:  [927626]                          0 Page Faults Major              0.000 /sec
stress-ng: info:  [927626]                  1,750,290 Context Switches              29.141 K/sec
stress-ng: info:  [927626]                      7,467 Cgroup Switches              124.322 /sec
stress-ng: info:  [927626]                        144 CPU Migrations                 2.398 /sec
stress-ng: info:  [927626]                          0 Alignment Faults               0.000 /sec
stress-ng: info:  [927626]                          0 Emulation Faults               0.000 /sec
stress-ng: info:  [927626] for a 60.06s run time:
stress-ng: info:  [927626]     240.25s available CPU time
stress-ng: info:  [927626]     132.08s user time   ( 54.98%)
stress-ng: info:  [927626]     105.14s system time ( 43.76%)
stress-ng: info:  [927626]     237.22s total time  ( 98.74%)
stress-ng: info:  [927626] load average: 11.50 4.14 2.48
stress-ng: info:  [927626] skipped: 0
stress-ng: info:  [927626] passed: 17: cpu (4) vm (2) fork (8) mq (3)
stress-ng: info:  [927626] failed: 0
stress-ng: info:  [927626] metrics untrustworthy: 0
stress-ng: info:  [927626] successful run completed in 1 min
```

**Analysis**: This final run is a "stress cocktail" that forces every subsystem—computational, memory, and kernel—to compete for the same finite resources on my four-core machine. With 17 total stressors dispatched, the load average hit 11.50, creating a massive execution bottleneck where the scheduler had to juggle nearly three times more tasks than available hardware threads. I can see that the fork and mq stressors are the biggest "thieves" of performance, together driving the system time up to 43.76% and generating over 10 million page faults and nearly 2 million context switches. Ultimately, this test proves that when the kernel is overwhelmed by high-frequency process creation and IPC, the efficiency of pure computational work and memory throughput degrades significantly due to the constant administrative overhead of context switching.

#### Questions

| Rank  | Stressor                  | System Time % | User Time % | Activity Profile     |
| ----- | ------------------------- | ------------- | ----------- | -------------------- |
| **1** | **MQ** (4 workers)        | **78.51%**    | 15.44%      | Kernel Dominant      |
| **2** | **Fork** (16 workers)     | **58.21%**    | 40.35%      | Kernel Dominant      |
| **3** | **Combined** (17 workers) | **43.76%**    | 54.98%      | Balanced/High Stress |
| **4** | **VM** (4 workers)        | **15.69%**    | 83.92%      | User Dominant        |
| **5** | **CPU** (4 workers)       | **0.23%**     | 99.46%      | User Dominant        |

**The Highest Kernel Activity:**
The **`--fork`** and **`--mq`** commands generate the highest kernel activity. Specifically, `fork` is the most taxing because it forces the CPU to spend its time inside the kernel's process management code. Every time a process is forked, the kernel must allocate a new Process ID (PID), copy memory page tables, and set up new task structures. Similarly, `mq` (Message Queue) requires the kernel to act as a postman, copying data between process buffers and managing synchronization locks. In these cases, the "work" isn't being done by the application, but by the OS managing the application's demands.

**Why the Memory Test has High CPU Utilization:** It might seem counterintuitive that a "Memory" test pins the CPU at nearly 100%, but this happens for two specific technical reasons:

- **Active Data Moving:** The VM stressor doesn't just "hold" memory; it actively writes to it. The CPU is responsible for executing the instructions that move data from registers into RAM addresses. To the operating system, a CPU core busy-looping through a memory array is just as "active" as a core doing math.
- **Virtual Memory Management:** Because you set a high `--vm-bytes` value, the CPU must constantly translate virtual addresses to physical addresses. If the stressor is frequently allocating and deallocating memory, it triggers a high rate of **Page Faults** (as seen in your `perf` output). The CPU must work to resolve these faults and manage the Translation Lookaside Buffer (TLB) to keep the data flowing.

### Using fio to issue I/O requests

Input: `fio --rw=write --bs=4k --size=1g --ioengine=sync --name=seqwr-bs4k-s1g`

Output:

```
seqwr-bs4k-s1g: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=sync, iodepth=1
fio-3.38
Starting 1 process
seqwr-bs4k-s1g: Laying out IO file (1 file / 1024MiB)

seqwr-bs4k-s1g: (groupid=0, jobs=1): err= 0: pid=1136899: Sat Mar 14 16:03:38 2026
  write: IOPS=419k, BW=1636MiB/s (1715MB/s)(1024MiB/626msec); 0 zone resets
    clat (nsec): min=1174, max=3048.5k, avg=2146.07, stdev=10708.97
     lat (nsec): min=1201, max=3048.5k, avg=2182.44, stdev=10711.02
    clat percentiles (nsec):
     |  1.00th=[  1256],  5.00th=[  1304], 10.00th=[  1352], 20.00th=[  1416],
     | 30.00th=[  1464], 40.00th=[  1560], 50.00th=[  1672], 60.00th=[  1800],
     | 70.00th=[  1928], 80.00th=[  2320], 90.00th=[  2928], 95.00th=[  3824],
     | 99.00th=[  7520], 99.50th=[ 10944], 99.90th=[ 27520], 99.95th=[ 38144],
     | 99.99th=[199680]
   bw (  MiB/s): min= 1686, max= 1686, per=100.00%, avg=1686.98, stdev= 0.00, samples=1
   iops        : min=431868, max=431868, avg=431868.00, stdev= 0.00, samples=1
  lat (usec)   : 2=73.06%, 4=22.41%, 10=3.93%, 20=0.42%, 50=0.15%
  lat (usec)   : 100=0.02%, 250=0.01%, 500=0.01%, 750=0.01%, 1000=0.01%
  lat (msec)   : 2=0.01%, 4=0.01%
  cpu          : usr=16.48%, sys=79.36%, ctx=112, majf=1, minf=10
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=1636MiB/s (1715MB/s), 1636MiB/s-1636MiB/s (1715MB/s-1715MB/s), io=1024MiB (1074MB), run=626-626msec

Disk stats (read/write):
  sda: ios=1/251, sectors=72/559104, merge=0/3, ticks=6/2200, in_queue=2206, util=22.65%
```

Input: `fio --rw=write --bs=4k --size=1g --ioengine=sync --name=seqwr-bs4k-s1g --direct=1`

Output:

```
seqwr-bs4k-s1g: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=sync, iodepth=1
fio-3.38
Starting 1 process
Jobs: 1 (f=1): [W(1)][100.0%][w=50.0MiB/s][w=12.8k IOPS][eta 00m:00s]
seqwr-bs4k-s1g: (groupid=0, jobs=1): err= 0: pid=1136922: Sat Mar 14 16:04:48 2026
  write: IOPS=16.6k, BW=64.7MiB/s (67.8MB/s)(1024MiB/15834msec); 0 zone resets
    clat (usec): min=32, max=2389, avg=59.63, stdev=33.34
     lat (usec): min=32, max=2389, avg=59.68, stdev=33.36
    clat percentiles (usec):
     |  1.00th=[   41],  5.00th=[   43], 10.00th=[   44], 20.00th=[   47],
     | 30.00th=[   49], 40.00th=[   51], 50.00th=[   53], 60.00th=[   56],
     | 70.00th=[   59], 80.00th=[   64], 90.00th=[   77], 95.00th=[   97],
     | 99.00th=[  186], 99.50th=[  235], 99.90th=[  461], 99.95th=[  553],
     | 99.99th=[  832]
   bw (  KiB/s): min=37936, max=79376, per=100.00%, avg=66253.16, stdev=9233.36, samples=31
   iops        : min= 9484, max=19844, avg=16563.29, stdev=2308.34, samples=31
  lat (usec)   : 50=36.73%, 100=58.74%, 250=4.11%, 500=0.34%, 750=0.07%
  lat (usec)   : 1000=0.01%
  lat (msec)   : 2=0.01%, 4=0.01%
  cpu          : usr=1.00%, sys=34.04%, ctx=262035, majf=0, minf=9
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=64.7MiB/s (67.8MB/s), 64.7MiB/s-64.7MiB/s (67.8MB/s-67.8MB/s), io=1024MiB (1074MB), run=15834-15834msec

Disk stats (read/write):
  sda: ios=0/258509, sectors=0/2068096, merge=0/3, ticks=0/13480, in_queue=13489, util=88.24%
```

**Throughput and IOPS:** The most immediate takeaway is the massive performance gap between the two runs.

| Metric             | Run 1 (Buffered) | Run 2 (Direct I/O) |
| ------------------ | ---------------- | ------------------ |
| **Bandwidth (BW)** | 1636 MiB/s       | 64.7 MiB/s         |
| **IOPS**           | 419,000          | 16,600             |
| **Runtime**        | 0.626 seconds    | 15.834 seconds     |

In the first run, the system achieved a throughput of 1.6 GiB/s. This is far too fast for a standard SATA or even many NVMe drives for a 4k sequential write; it indicates that the data was simply being written to the **Linux Page Cache (RAM)**. In the second run, using `--direct=1` forced the data to go straight to the disk controller. The result—64.7 MiB/s—is the actual physical write speed of my storage device for a 4k block size.

**Latency and CPU Dynamics:** The latency profiles show exactly why buffered I/O feels faster to a user but is more "dangerous" for data integrity:

- **Average Latency:** In the buffered run, average latency was a mere **2.1 microseconds** ($\text{avg}=2146.07 \text{ nsec}$). In the direct run, it jumped to **59.6 microseconds**. The buffered run is essentially measuring the speed of my memory bus, while the direct run is measuring the physical seek/write time of the drive.
- **CPU Usage:** The buffered test used **79.36% system time**, as the kernel was working overtime to manage the page cache and memory mappings. In the direct I/O test, system time dropped to **34.04%**, as the kernel was bypassed, leaving the hardware to handle the heavy lifting.
- **Context Switches:** The direct I/O run saw a massive spike in context switches (**262,035** vs. **112**). This happens because the process must constantly wait for the physical hardware to acknowledge each write before the next `sync` operation can proceed, causing the scheduler to swap tasks in and out.

**Physical Disk Utilization:** Looking at the `Disk stats` at the bottom of the output confirms the cache behavior:

- **Buffered Run:** Disk utility was only **22.65%**. The run finished so fast (626ms) that the physical disk (`sda`) didn't even have time to catch up with the data sitting in RAM.
- **Direct Run:** Disk utility was **88.24%**. This shows that for nearly the entire 15-second duration, the disk was working at near-maximum capacity to process the incoming 4k blocks.

### Using fio to evaluate the impact of tracers

**Version without any tracer**:

```
seqwr-bs4k-s1g: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=sync, iodepth=1
fio-3.38
Starting 1 process
seqwr-bs4k-s1g: Laying out IO file (1 file / 1024MiB)

seqwr-bs4k-s1g: (groupid=0, jobs=1): err= 0: pid=2799: Sat Mar 14 18:38:28 2026
  write: IOPS=331k, BW=1295MiB/s (1357MB/s)(1024MiB/791msec); 0 zone resets
    clat (nsec): min=994, max=2851.8k, avg=2799.71, stdev=23126.57
     lat (nsec): min=1018, max=2851.8k, avg=2833.89, stdev=23127.28
    clat percentiles (nsec):
     |  1.00th=[  1064],  5.00th=[  1080], 10.00th=[  1112], 20.00th=[  1192],
     | 30.00th=[  1240], 40.00th=[  1304], 50.00th=[  1384], 60.00th=[  1528],
     | 70.00th=[  1704], 80.00th=[  1992], 90.00th=[  2672], 95.00th=[  3536],
     | 99.00th=[  8512], 99.50th=[ 17536], 99.90th=[419840], 99.95th=[485376],
     | 99.99th=[675840]
   bw (  MiB/s): min= 1369, max= 1369, per=100.00%, avg=1369.49, stdev= 0.00, samples=1
   iops        : min=350590, max=350590, avg=350590.00, stdev= 0.00, samples=1
  lat (nsec)   : 1000=0.01%
  lat (usec)   : 2=80.34%, 4=15.79%, 10=3.02%, 20=0.39%, 50=0.20%
  lat (usec)   : 100=0.03%, 250=0.02%, 500=0.17%, 750=0.04%, 1000=0.01%
  lat (msec)   : 2=0.01%, 4=0.01%
  cpu          : usr=15.57%, sys=82.15%, ctx=82, majf=1, minf=8
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=1295MiB/s (1357MB/s), 1295MiB/s-1295MiB/s (1357MB/s-1357MB/s), io=1024MiB (1074MB), run=791-791msec

Disk stats (read/write):
  sda: ios=1/370, sectors=72/850944, merge=0/0, ticks=1/5345, in_queue=5348, util=24.66%
```

**Time** output:

```
real	0m2.854s
user	0m0.236s
sys 	0m0.725s
```

**Version with `strace`**:

```
seqwr-bs4k-s1g: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=sync, iodepth=1
fio-3.38
Starting 1 process
seqwr-bs4k-s1g: Laying out IO file (1 file / 1024MiB)
Jobs: 1 (f=1): [W(1)][100.0%][w=68.7MiB/s][w=17.6k IOPS][eta 00m:00s]
seqwr-bs4k-s1g: (groupid=0, jobs=1): err= 0: pid=2911: Sat Mar 14 18:42:03 2026
  write: IOPS=12.3k, BW=48.2MiB/s (50.5MB/s)(1024MiB/21248msec); 0 zone resets
    clat (usec): min=11, max=5792, avg=80.51, stdev=69.96
     lat (usec): min=11, max=5794, avg=80.58, stdev=70.00
    clat percentiles (usec):
     |  1.00th=[   20],  5.00th=[   25], 10.00th=[   27], 20.00th=[   30],
     | 30.00th=[   34], 40.00th=[   45], 50.00th=[   72], 60.00th=[   82],
     | 70.00th=[   95], 80.00th=[  120], 90.00th=[  159], 95.00th=[  192],
     | 99.00th=[  285], 99.50th=[  347], 99.90th=[  578], 99.95th=[  766],
     | 99.99th=[ 1909]
   bw (  KiB/s): min=23578, max=95672, per=100.00%, avg=49378.10, stdev=16637.23, samples=42
   iops        : min= 5894, max=23918, avg=12344.26, stdev=4159.37, samples=42
  lat (usec)   : 20=1.15%, 50=40.70%, 100=31.35%, 250=25.12%, 500=1.54%
  lat (usec)   : 750=0.09%, 1000=0.02%
  lat (msec)   : 2=0.02%, 4=0.01%, 10=0.01%
  cpu          : usr=1.76%, sys=24.56%, ctx=524512, majf=0, minf=12
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=48.2MiB/s (50.5MB/s), 48.2MiB/s-48.2MiB/s (50.5MB/s-50.5MB/s), io=1024MiB (1074MB), run=21248-21248msec

Disk stats (read/write):
  sda: ios=0/537, sectors=0/1229312, merge=0/56, ticks=0/3159, in_queue=3187, util=1.54%
```

**Time** output:

```
real	0m35.799s
user	0m0.542s
sys 	0m5.654s
```

**Version with `systracer`**:

```
seqwr-bs4k-s1g: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=sync, iodepth=1
fio-3.38
Starting 1 process
Jobs: 1 (f=1)
seqwr-bs4k-s1g: (groupid=0, jobs=1): err= 0: pid=2988: Sat Mar 14 18:44:17 2026
  write: IOPS=97.4k, BW=380MiB/s (399MB/s)(1024MiB/2692msec); 0 zone resets
    clat (nsec): min=1609, max=5748.7k, avg=8526.57, stdev=39132.02
     lat (nsec): min=1636, max=5748.8k, avg=8632.60, stdev=39334.29
    clat percentiles (usec):
     |  1.00th=[    3],  5.00th=[    3], 10.00th=[    3], 20.00th=[    3],
     | 30.00th=[    3], 40.00th=[    3], 50.00th=[    3], 60.00th=[    4],
     | 70.00th=[    4], 80.00th=[    5], 90.00th=[    7], 95.00th=[   37],
     | 99.00th=[   91], 99.50th=[  165], 99.90th=[  578], 99.95th=[  758],
     | 99.99th=[ 1254]
   bw (  KiB/s): min=143808, max=675344, per=95.66%, avg=372594.60, stdev=235803.44, samples=5
   iops        : min=35952, max=168836, avg=93148.60, stdev=58950.81, samples=5
  lat (usec)   : 2=0.43%, 4=69.67%, 10=22.16%, 20=0.92%, 50=4.48%
  lat (usec)   : 100=1.49%, 250=0.50%, 500=0.22%, 750=0.08%, 1000=0.03%
  lat (msec)   : 2=0.02%, 4=0.01%, 10=0.01%
  cpu          : usr=7.32%, sys=88.67%, ctx=366, majf=0, minf=11
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=380MiB/s (399MB/s), 380MiB/s-380MiB/s (399MB/s-399MB/s), io=1024MiB (1074MB), run=2692-2692msec

Disk stats (read/write):
  sda: ios=0/761, sectors=0/1663360, merge=0/10, ticks=0/5720, in_queue=5726, util=15.70%
```

**Time** output:

```
real	0m8.933s
user	0m0.292s
sys 	0m2.484s
```

To compare the overhead, we look at the **Throughput (BW)**, **IOPS**, and the **Total Real Time** from the `time` command.

| Metric              | Baseline (No Tracer) | eBPF (systracer) | strace     |
| ------------------- | -------------------- | ---------------- | ---------- |
| **Throughput (BW)** | 1295 MiB/s           | 380 MiB/s        | 48.2 MiB/s |
| **IOPS**            | 331k                 | 97.4k            | 12.3k      |
| **Real Time**       | 2.854s               | 8.933s           | 35.799s    |
| **Slowdown (Time)** | 1x (Baseline)        | **~3.1x**        | **~12.5x** |

**strace imposes significantly higher overhead.** The reason for this massive disparity lies in the architecture of the tracers:

1. **strace (The "Stop-and-Go" approach):** Every time `fio` makes a system call, the kernel pauses the `fio` process and sends a signal to `strace`. `strace` then context switches in, reads the data, and context switches out so the kernel can resume `fio`. For a workload doing 331k IOPS, this creates millions of context switches, which is why your IOPS dropped by over **96%**.
2. **eBPF / systracer (The "In-Kernel" approach):** eBPF runs your tracing code directly inside the kernel context. There is no context switching for every system call. It simply records the data into a high-speed buffer. While it still adds some "instruction overhead" (dropping performance by about 70% in this high-frequency case), it is still **~8x faster** than strace in this specific test.
