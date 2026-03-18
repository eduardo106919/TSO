# Guide 05

## Version with FUSE

**FIO** result:

```
PID = 57043
Press any key to continue...
seq_write: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=psync, iodepth=1
fio-3.36
Starting 1 process
Jobs: 1 (f=1): [W(1)][100.0%][w=197MiB/s][w=50.4k IOPS][eta 00m:00s]
seq_write: (groupid=0, jobs=1): err= 0: pid=57288: Wed Mar 18 22:09:14 2026
  write: IOPS=42.8k, BW=167MiB/s (176MB/s)(1024MiB/6118msec); 0 zone resets
    clat (usec): min=11, max=3996, avg=22.99, stdev=17.31
     lat (usec): min=11, max=3996, avg=23.04, stdev=17.33
    clat percentiles (usec):
     |  1.00th=[   12],  5.00th=[   13], 10.00th=[   13], 20.00th=[   13],
     | 30.00th=[   15], 40.00th=[   21], 50.00th=[   23], 60.00th=[   24],
     | 70.00th=[   24], 80.00th=[   26], 90.00th=[   33], 95.00th=[   45],
     | 99.00th=[   80], 99.50th=[   99], 99.90th=[  167], 99.95th=[  202],
     | 99.99th=[  322]
   bw (  KiB/s): min=72416, max=290856, per=100.00%, avg=171891.33, stdev=55764.25, samples=12
   iops        : min=18104, max=72714, avg=42972.83, stdev=13941.06, samples=12
  lat (usec)   : 20=39.73%, 50=56.45%, 100=3.34%, 250=0.47%, 500=0.02%
  lat (usec)   : 750=0.01%, 1000=0.01%
  lat (msec)   : 2=0.01%, 4=0.01%
  cpu          : usr=3.47%, sys=20.34%, ctx=262161, majf=0, minf=9
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=167MiB/s (176MB/s), 167MiB/s-167MiB/s (176MB/s-176MB/s), io=1024MiB (1074MB), run=6118-6118msec
```

**Syscounter** result:

```
PID\OP                        openat          close           read            pwrite          write           pread
------------------------------------------------------------------------------------------------------------------------------
fio-57288                     2               2               1               262144          0               0
script.sh-57043               0               0               1               0               0               0
fio-57260                     1437            131             137             0               0               2
script.sh-57260               0               1               0               0               0               0
passthrough-55449             1               1               0               262144          262146          0
------------------------------------------------------------------------------------------------------------------------------
Total                         1440            135             139             524288          262146          2
```

## Version without FUSE

**FIO** result:

```
seq_write: (g=0): rw=write, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=psync, iodepth=1
fio-3.36
Starting 1 process
seq_write: Laying out IO file (1 file / 1024MiB)

seq_write: (groupid=0, jobs=1): err= 0: pid=58846: Wed Mar 18 22:12:05 2026
  write: IOPS=567k, BW=2216MiB/s (2324MB/s)(1024MiB/462msec); 0 zone resets
    clat (nsec): min=1098, max=274208, avg=1599.09, stdev=2010.07
     lat (nsec): min=1125, max=274265, avg=1623.25, stdev=2013.50
    clat percentiles (nsec):
     |  1.00th=[ 1176],  5.00th=[ 1224], 10.00th=[ 1256], 20.00th=[ 1288],
     | 30.00th=[ 1320], 40.00th=[ 1336], 50.00th=[ 1368], 60.00th=[ 1400],
     | 70.00th=[ 1448], 80.00th=[ 1528], 90.00th=[ 2288], 95.00th=[ 2704],
     | 99.00th=[ 4384], 99.50th=[ 5728], 99.90th=[15040], 99.95th=[33536],
     | 99.99th=[63232]
  lat (usec)   : 2=88.08%, 4=10.78%, 10=0.97%, 20=0.09%, 50=0.06%
  lat (usec)   : 100=0.02%, 250=0.01%, 500=0.01%
  cpu          : usr=14.97%, sys=85.03%, ctx=5, majf=0, minf=13
  IO depths    : 1=100.0%, 2=0.0%, 4=0.0%, 8=0.0%, 16=0.0%, 32=0.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     issued rwts: total=0,262144,0,0 short=0,0,0,0 dropped=0,0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=1

Run status group 0 (all jobs):
  WRITE: bw=2216MiB/s (2324MB/s), 2216MiB/s-2216MiB/s (2324MB/s-2324MB/s), io=1024MiB (1074MB), run=462-462msec

Disk stats (read/write):
  nvme0n1: ios=1/0, sectors=256/0, merge=0/0, ticks=6/0, in_queue=6, util=1.21%
```

**Syscounter** result:

```
PID\OP                        pwrite          openat          read            close           pread
--------------------------------------------------------------------------------------------------------------
fio-58846                     262144          2               1               2               0
fio-58828                     0               242             94              89              2
script.sh-58725               0               0               1               0               0
script.sh-58828               0               0               0               1               0
--------------------------------------------------------------------------------------------------------------
Total                         262144          244             96              92              2
```

**Analysis**: we can see that the number of `pwrite` is double with FUSE. This happens because the system call goes from User Space into Kernel, then it goes into User Space again for the FUSE daemon and finally into Kernel.
