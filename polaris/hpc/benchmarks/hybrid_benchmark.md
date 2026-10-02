# Polaris Hybrid MPI + OpenMP Benchmark

## 1. Overview

This document presents the benchmarking results of the hybrid MPI + OpenMP implementation developed for the Polaris HPC job-log analytics workload.

The objective is to evaluate how the workload scales with different combinations of:

- MPI processes
- OpenMP threads per MPI process
- Single-node execution
- Two-node distributed execution
- Dataset size

The benchmark uses the Polaris datasets ranging from 160 MB to 10 GB.

Each configuration was executed five times and the arithmetic mean of the five measurements is used for comparison.

The hybrid implementation combines:

- MPI for distributed-memory parallelism
- OpenMP for shared-memory parallelism within each MPI process

The two compute nodes used for distributed experiments were:

| Node | CPU | Physical Cores | Threads |
|---|---|---:|---:|
| Master | Intel i5-12450HX | 8 | 12 |
| Worker | Intel i5-3570 | 4 | 4 |

Therefore, the benchmark explores configurations appropriate to the available hardware rather than assuming that a single MPI/OpenMP decomposition is optimal for every workload size.

---

# 2. Benchmark Methodology

## 2.1 Dataset sizes

The following datasets were benchmarked:

| Dataset |
|---|
| 160 MB |
| 500 MB |
| 1 GB |
| 2 GB |
| 4 GB |
| 8 GB |
| 10 GB |

---

## 2.2 Measurements

For every configuration:

1. The program was executed five times.
2. Wall-clock execution time was measured using `/usr/bin/time`.
3. The arithmetic mean of the five runs was calculated.
4. The minimum mean runtime was selected as the best configuration for that dataset.

No individual run was discarded merely because it was slower than the others.

This preserves the raw experimental behavior and avoids selectively removing measurements.

---

# 3. Configuration Space

## 3.1 Master-only configurations

The master contains 8 physical cores and 12 hardware threads.

The following hybrid configurations were tested:

| MPI Processes | OpenMP Threads/Rank | Total OpenMP Workers |
|---:|---:|---:|
| 1 | 12 | 12 |
| 2 | 6 | 12 |
| 3 | 4 | 12 |
| 4 | 3 | 12 |
| 6 | 2 | 12 |
| 8 | 1 | 8 |

This allows the effect of changing the MPI/OpenMP decomposition while keeping approximately the same total worker count in most configurations.

---

## 3.2 Worker-only configurations

The worker contains 4 physical cores and 4 hardware threads.

The following configurations were tested:

| MPI Processes | OpenMP Threads/Rank |
|---:|---:|
| 1 | 4 |
| 2 | 2 |
| 4 | 1 |

---

## 3.3 Two-node configurations

The two-node experiments distributed MPI processes between the master and worker.

The following configurations were tested:

| Total MPI | Master | Worker | OMP Threads/Rank |
|---:|---:|---:|---:|
| 2 | 1 | 1 | 1 |
| 2 | 1 | 1 | 2 |
| 2 | 1 | 1 | 4 |
| 4 | 2 | 2 | 1 |
| 4 | 2 | 2 | 2 |
| 6 | 4 | 2 | 1 |
| 6 | 4 | 2 | 2 |
| 8 | 6 | 2 | 1 |
| 12 | 8 | 4 | 1 |

The asymmetric distributions reflect the difference in computational capacity between the two machines.

---

# 4. Serial Baseline

The serial implementation was benchmarked separately using five runs for each dataset.

| Dataset | Serial Mean (s) |
|---|---:|
| 160 MB | 0.326 |
| 500 MB | 0.986 |
| 1 GB | 2.060 |
| 2 GB | 4.014 |
| 4 GB | 8.272 |
| 8 GB | 17.442 |
| 10 GB | 21.908 |

The serial mean is used as the baseline for calculating hybrid speedup.

---

# 5. Master-Only Hybrid Results

## 5.1 160 MB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 12 | **0.608** |
| 2 × 6 | 0.696 |
| 3 × 4 | 0.658 |
| 4 × 3 | 0.688 |
| 6 × 2 | 0.712 |
| 8 × 1 | 0.794 |

**Best:** 1 MPI × 12 OMP — **0.608 s**

---

## 5.2 500 MB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 12 | **0.772** |
| 2 × 6 | 1.052 |
| 3 × 4 | 0.806 |
| 4 × 3 | 0.858 |
| 6 × 2 | 0.866 |
| 8 × 1 | 0.900 |

**Best:** 1 MPI × 12 OMP — **0.772 s**

---

## 5.3 1 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 12 | **0.976** |
| 2 × 6 | 1.582 |
| 3 × 4 | 1.102 |
| 4 × 3 | 1.092 |
| 6 × 2 | 1.118 |
| 8 × 1 | 1.152 |

**Best:** 1 MPI × 12 OMP — **0.976 s**

---

## 5.4 2 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 12 | **1.434** |
| 2 × 6 | 2.664 |
| 3 × 4 | 1.520 |
| 4 × 3 | 1.504 |
| 6 × 2 | 1.546 |
| 8 × 1 | 1.632 |

**Best:** 1 MPI × 12 OMP — **1.434 s**

---

## 5.5 4 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 12 | **2.116** |
| 2 × 6 | 4.612 |
| 3 × 4 | 2.138 |
| 4 × 3 | 2.146 |
| 6 × 2 | 2.160 |
| 8 × 1 | 2.478 |

**Best:** 1 MPI × 12 OMP — **2.116 s**

---

## 5.6 8 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 12 | 4.436 |
| 2 × 6 | 9.122 |
| 3 × 4 | 4.368 |
| **4 × 3** | **4.262** |
| 6 × 2 | 4.328 |
| 8 × 1 | 4.646 |

**Best:** 4 MPI × 3 OMP — **4.262 s**

---

## 5.7 10 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| **1 × 12** | **5.102** |
| 2 × 6 | 11.226 |
| 3 × 4 | 5.542 |
| 4 × 3 | 5.194 |
| 6 × 2 | 5.614 |
| 8 × 1 | 6.040 |

**Best:** 1 MPI × 12 OMP — **5.102 s**

---

# 6. Worker-Only Hybrid Results

## 6.1 160 MB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| **1 × 4** | **0.482** |
| 2 × 2 | 0.630 |
| 4 × 1 | 0.494 |

---

## 6.2 500 MB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| **1 × 4** | **0.860** |
| 2 × 2 | 1.306 |
| 4 × 1 | 0.864 |

---

## 6.3 1 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| **1 × 4** | **1.452** |
| 2 × 2 | 2.356 |
| 4 × 1 | 1.440 |

---

## 6.4 2 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| **1 × 4** | **2.580** |
| 2 × 2 | 4.388 |
| 4 × 1 | 2.002 |

---

## 6.5 4 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 4 | 4.878 |
| 2 × 2 | 8.486 |
| **4 × 1** | **4.798** |

---

## 6.6 8 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 4 | 9.480 |
| 2 × 2 | 16.656 |
| **4 × 1** | **9.292** |

---

## 6.7 10 GB

| MPI × OMP | Mean Runtime (s) |
|---|---:|
| 1 × 4 | 19.234 |
| 2 × 2 | 27.606 |
| **4 × 1** | **17.936** |

---

# 7. Two-Node Hybrid Results

## 7.1 160 MB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 1.186 |
| 2 × 2 | 1M + 1W | 1.184 |
| 2 × 4 | 1M + 1W | 1.168 |
| 4 × 1 | 2M + 2W | 1.102 |
| **4 × 2** | **2M + 2W** | **1.014** |
| 6 × 1 | 4M + 2W | 1.062 |
| 6 × 2 | 4M + 2W | 1.058 |
| 8 × 1 | 6M + 2W | 1.116 |
| 12 × 1 | 8M + 4W | 1.168 |

**Best:** 4 MPI × 2 OMP — **1.014 s**

---

## 7.2 500 MB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 1.852 |
| 2 × 2 | 1M + 1W | 1.834 |
| 2 × 4 | 1M + 1W | 1.840 |
| 4 × 1 | 2M + 2W | 1.416 |
| **4 × 2** | **2M + 2W** | **1.172** |
| 6 × 1 | 4M + 2W | 1.308 |
| **6 × 2** | **4M + 2W** | **1.138** |
| 8 × 1 | 6M + 2W | 1.212 |
| 12 × 1 | 8M + 4W | 1.274 |

**Best:** 6 MPI × 2 OMP — **1.138 s**

---

## 7.3 1 GB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 2.906 |
| 2 × 2 | 1M + 1W | 2.888 |
| 2 × 4 | 1M + 1W | 2.928 |
| 4 × 1 | 2M + 2W | 1.954 |
| 4 × 2 | 2M + 2W | 1.484 |
| 6 × 1 | 4M + 2W | 1.662 |
| **6 × 2** | **4M + 2W** | **1.332** |
| 8 × 1 | 6M + 2W | 1.568 |
| 12 × 1 | 8M + 4W | 1.436 |

**Best:** 6 MPI × 2 OMP — **1.332 s**

---

## 7.4 2 GB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 4.930 |
| 2 × 2 | 1M + 1W | 4.922 |
| 2 × 4 | 1M + 1W | 4.946 |
| 4 × 1 | 2M + 2W | 2.992 |
| 4 × 2 | 2M + 2W | 2.044 |
| 6 × 1 | 4M + 2W | 2.354 |
| **6 × 2** | **4M + 2W** | **1.716** |
| 8 × 1 | 6M + 2W | 2.100 |
| 12 × 1 | 8M + 4W | 1.840 |

**Best:** 6 MPI × 2 OMP — **1.716 s**

---

## 7.5 4 GB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 8.966 |
| 2 × 2 | 1M + 1W | 8.958 |
| 2 × 4 | 1M + 1W | 8.964 |
| 4 × 1 | 2M + 2W | 5.018 |
| 4 × 2 | 2M + 2W | 3.162 |
| 6 × 1 | 4M + 2W | 3.700 |
| 6 × 2 | 4M + 2W | 2.434 |
| 8 × 1 | 6M + 2W | 3.054 |
| **12 × 1** | **8M + 4W** | **2.594** |

**Best:** 12 MPI × 1 OMP — **2.594 s**

---

## 7.6 8 GB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 17.048 |
| 2 × 2 | 1M + 1W | 17.062 |
| 2 × 4 | 1M + 1W | 17.252 |
| 4 × 1 | 2M + 2W | 9.118 |
| 4 × 2 | 2M + 2W | 5.386 |
| 6 × 1 | 4M + 2W | 6.396 |
| **6 × 2** | **4M + 2W** | **3.936** |
| 8 × 1 | 6M + 2W | 5.118 |
| 12 × 1 | 8M + 4W | 4.006 |

**Best:** 6 MPI × 2 OMP — **3.936 s**

---

## 7.7 10 GB

| MPI × OMP | Distribution | Mean Runtime (s) |
|---|---|---:|
| 2 × 1 | 1M + 1W | 21.696 |
| 2 × 2 | 1M + 1W | 21.016 |
| 2 × 4 | 1M + 1W | 21.028 |
| 4 × 1 | 2M + 2W | 11.134 |
| 4 × 2 | 2M + 2W | 6.606 |
| 6 × 1 | 4M + 2W | 7.804 |
| **6 × 2** | **4M + 2W** | **4.704** |
| 8 × 1 | 6M + 2W | 6.224 |
| 12 × 1 | 8M + 4W | 4.842 |

**Best:** 6 MPI × 2 OMP — **4.704 s**

---

# 8. Best Hybrid Configuration

The best configuration is selected from **both single-node and two-node hybrid experiments**.

| Dataset | Best Configuration | Runtime (s) | Execution |
|---|---|---:|---|
| 160 MB | 1 MPI × 12 OMP | 0.608 | Master |
| 500 MB | 1 MPI × 12 OMP | 0.772 | Master |
| 1 GB | 1 MPI × 12 OMP | 0.976 | Master |
| 2 GB | 1 MPI × 12 OMP | 1.434 | Master |
| 4 GB | 1 MPI × 12 OMP | 2.116 | Master |
| 8 GB | 6 MPI × 2 OMP | **3.936** | Two-node |
| 10 GB | 6 MPI × 2 OMP | **4.704** | Two-node |

---

# 9. Hybrid Speedup Over Serial

Speedup is calculated as:

\[
S = \frac{T_{serial}}{T_{hybrid}}
\]

where:

- \(T_{serial}\) = mean runtime of the serial implementation
- \(T_{hybrid}\) = mean runtime of the best hybrid configuration

| Dataset | Serial (s) | Best Hybrid (s) | Best Configuration | Speedup |
|---|---:|---:|---|---:|
| 160 MB | 0.326 | 0.608 | 1×12 | 0.536× |
| 500 MB | 0.986 | 0.772 | 1×12 | **1.277×** |
| 1 GB | 2.060 | 0.976 | 1×12 | **2.111×** |
| 2 GB | 4.014 | 1.434 | 1×12 | **2.799×** |
| 4 GB | 8.272 | 2.116 | 1×12 | **3.909×** |
| 8 GB | 17.442 | 3.936 | 6×2 | **4.431×** |
| 10 GB | 21.908 | 4.704 | 6×2 | **4.657×** |

The 160 MB result has a speedup below 1 because the hybrid runtime is longer than the serial runtime. For such a small workload, parallel execution overhead dominates the actual computation.

---

# 10. Hybrid Runtime Reduction

Runtime reduction relative to serial execution is:

\[
R = \left(1-\frac{T_{hybrid}}{T_{serial}}\right)\times100
\]

| Dataset | Runtime Reduction |
|---|---:|
| 160 MB | -86.5% |
| 500 MB | 21.7% |
| 1 GB | 52.6% |
| 2 GB | 64.3% |
| 4 GB | 74.4% |
| 8 GB | 77.4% |
| 10 GB | 78.5% |

---

# 11. Single-Node vs Two-Node Hybrid

The best single-node hybrid configuration and best two-node configuration can be compared directly.

| Dataset | Best Single-Node (s) | Best Two-Node (s) | Better |
|---|---:|---:|---|
| 160 MB | 0.608 | 1.014 | Single-node |
| 500 MB | 0.772 | 1.138 | Single-node |
| 1 GB | 0.976 | 1.332 | Single-node |
| 2 GB | 1.434 | 1.716 | Single-node |
| 4 GB | 2.116 | 2.434 | Single-node |
| 8 GB | 4.262 | **3.936** | **Two-node** |
| 10 GB | 5.102 | **4.704** | **Two-node** |

The results show a clear crossover.

For datasets up to 4 GB, the additional inter-node communication and distributed execution overhead outweigh the computational benefit of the second machine.

At 8 GB, the two-node configuration becomes faster.

At 10 GB, the two-node configuration remains faster.

---

# 12. Two-Node Improvement at Large Dataset Sizes

## 8 GB

Best single-node:

\[
T_{single}=4.262s
\]

Best two-node:

\[
T_{two-node}=3.936s
\]

Speedup from adding the second node:

\[
S_{2node}=\frac{4.262}{3.936}=1.083
\]

Therefore, the two-node configuration is approximately **8.3% faster**.

---

## 10 GB

Best single-node:

\[
T_{single}=5.102s
\]

Best two-node:

\[
T_{two-node}=4.704s
\]

\[
S_{2node}=\frac{5.102}{4.704}=1.085
\]

Therefore, the two-node configuration is approximately **8.5% faster**.

---

# 13. Observations

## 13.1 Dataset size strongly affects the optimal configuration

The best configuration does not remain constant as the dataset grows.

For smaller datasets, a single MPI process with many OpenMP threads generally performs best.

For larger datasets, distributed MPI execution becomes increasingly competitive.

The optimal configuration changes from:

```text
1 MPI × 12 OMP
