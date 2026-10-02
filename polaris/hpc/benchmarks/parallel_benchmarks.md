# Polaris HPC Log Analytics
# Parallel Performance Benchmark

## 1. Experimental Setup

The Polaris job-log analytics application was evaluated using:

1. Serial execution
2. OpenMP shared-memory parallel execution
3. MPI single-node execution
4. MPI two-node distributed execution

The datasets ranged from 160 MB to 10 GB.

### Master Node

- CPU: Intel Core i5-12450HX
- Physical cores: 8
- Logical CPUs: 12

### Worker Node

- CPU: Intel Core i5-3570
- Physical cores: 4
- Logical CPUs: 4

### Two-node MPI process distribution

| Total Processes | Master | Worker |
|---:|---:|---:|
| 2 | 1 | 1 |
| 4 | 2 | 2 |
| 8 | 4 | 4 |
| 12 | 8 | 4 |

Each configuration was executed repeatedly. Mean wall-clock runtime was used
for the benchmark comparisons.

---

# 2. Serial Benchmark

| Dataset | Mean Runtime (s) |
|---|---:|
| 160 MB | 0.326 |
| 500 MB | 0.986 |
| 1 GB | 2.060 |
| 2 GB | 4.014 |
| 4 GB | 8.272 |
| 8 GB | 17.442 |
| 10 GB | 21.908 |

The serial implementation provides the baseline against which the parallel
implementations are evaluated.

---

# 3. OpenMP Benchmark

Mean wall-clock runtime in seconds.

| Dataset | 1 Thread | 2 Threads | 4 Threads | 8 Threads | 12 Threads |
|---|---:|---:|---:|---:|---:|
| 160 MB | 0.356 | 0.204 | 0.122 | 0.088 | **0.084** |
| 500 MB | 1.138 | 0.592 | 0.340 | 0.246 | **0.230** |
| 1 GB | 2.298 | 1.208 | 0.654 | 0.472 | **0.446** |
| 2 GB | 4.518 | 2.372 | 1.282 | 0.914 | **0.806** |
| 4 GB | 9.172 | 4.866 | 2.616 | 1.818 | **1.626** |
| 8 GB | 21.604 | 12.660 | 7.528 | 5.702 | **5.258** |
| 10 GB | 24.822 | 14.310 | 8.988 | 6.899 | **6.136** |

OpenMP provides strong shared-memory scaling. Increasing the thread count
substantially reduces runtime, although the improvement becomes smaller at
higher thread counts.

---

# 4. Single-Node MPI Benchmark

Mean wall-clock runtime in seconds.

| Dataset | 1 Process | 2 Processes | 4 Processes | 8 Processes | 12 Processes |
|---|---:|---:|---:|---:|---:|
| 160 MB | 0.876 | 0.690 | **0.636** | 0.750 | 0.878 |
| 500 MB | 1.652 | 1.066 | **0.880** | 0.892 | 1.078 |
| 1 GB | 2.796 | 1.606 | **1.144** | 1.164 | 1.282 |
| 2 GB | 5.058 | 2.822 | 1.914 | **1.628** | 1.702 |
| 4 GB | 9.628 | 5.146 | 3.052 | 2.666 | **2.544** |
| 8 GB | 20.192 | 10.732 | 6.072 | 4.852 | **4.442** |
| 10 GB | 25.116 | 13.304 | 7.390 | 5.958 | **5.406** |

Single-node MPI becomes increasingly effective as the dataset size increases.
Higher process counts become useful for larger datasets.

---

# 5. Two-Node MPI Benchmark

Mean wall-clock runtime in seconds.

| Dataset | 2 Processes | 4 Processes | 8 Processes | 12 Processes |
|---|---:|---:|---:|---:|
| 160 MB | 1.172 | 1.072 | 0.976 | 1.054 |
| 500 MB | 1.844 | 1.390 | 1.186 | 1.210 |
| 1 GB | 2.906 | 1.916 | 1.454 | **1.374** |
| 2 GB | 4.924 | 2.900 | 2.026 | **1.744** |
| 4 GB | 8.914 | 4.866 | 3.094 | **2.502** |
| 8 GB | 16.218 | 8.800 | 5.258 | **3.994** |
| 10 GB | 21.154 | 10.750 | 6.320 | **4.780** |

The 12-process configuration becomes increasingly advantageous for larger
datasets because the workload is distributed across both physical machines.

---

# 6. Best Configuration

| Dataset | Serial | Best OpenMP | Best 1-Node MPI | Best 2-Node MPI |
|---|---:|---:|---:|---:|
| 160 MB | 0.326 | **0.084** | 0.636 | 0.976 |
| 500 MB | 0.986 | **0.230** | 0.880 | 1.186 |
| 1 GB | 2.060 | **0.446** | 1.144 | 1.374 |
| 2 GB | 4.014 | **0.806** | 1.628 | 1.744 |
| 4 GB | 8.272 | **1.626** | 2.544 | 2.502 |
| 8 GB | 17.442 | 5.258 | 4.442 | **3.994** |
| 10 GB | 21.908 | 6.136 | 5.406 | **4.780** |

---

# 7. Serial-to-Parallel Speedup

Speedup is calculated as:

    Speedup = Serial Runtime / Parallel Runtime

### Best OpenMP

| Dataset | Serial | Best OpenMP | Speedup |
|---|---:|---:|---:|
| 160 MB | 0.326 | 0.084 | **3.88x** |
| 500 MB | 0.986 | 0.230 | **4.29x** |
| 1 GB | 2.060 | 0.446 | **4.62x** |
| 2 GB | 4.014 | 0.806 | **4.98x** |
| 4 GB | 8.272 | 1.626 | **5.09x** |
| 8 GB | 17.442 | 5.258 | **3.32x** |
| 10 GB | 21.908 | 6.136 | **3.57x** |

OpenMP achieves its strongest measured speedup at 4 GB, reaching
approximately 5.09x over the serial implementation.

---

# 8. Single-Node MPI Speedup

Speedup is calculated relative to the one-process MPI execution.

| Dataset | 2 Processes | 4 Processes | 8 Processes | 12 Processes |
|---|---:|---:|---:|---:|
| 160 MB | 1.27x | **1.38x** | 1.17x | 1.00x |
| 500 MB | 1.55x | **1.88x** | 1.85x | 1.53x |
| 1 GB | 1.74x | **2.44x** | 2.40x | 2.18x |
| 2 GB | 1.79x | 2.64x | **3.11x** | 2.97x |
| 4 GB | 1.87x | 3.15x | 3.61x | **3.78x** |
| 8 GB | 1.88x | 3.33x | 4.16x | **4.55x** |
| 10 GB | 1.89x | 3.40x | 4.22x | **4.65x** |

MPI scaling improves with increasing dataset size.

---

# 9. Best Two-Node MPI vs Best Single-Node MPI

| Dataset | Best 1-Node MPI | Best 2-Node MPI | 2-Node Improvement |
|---|---:|---:|---:|
| 160 MB | 0.636 | 0.976 | -53.5% |
| 500 MB | 0.880 | 1.186 | -34.8% |
| 1 GB | 1.144 | 1.374 | -20.1% |
| 2 GB | 1.628 | 1.744 | -7.1% |
| 4 GB | 2.544 | 2.502 | +1.7% |
| 8 GB | 4.442 | 3.994 | +10.1% |
| 10 GB | 5.406 | 4.780 | +11.6% |

Positive values indicate that the two-node configuration is faster.

The results show a clear workload-size dependency.

For smaller datasets, distributed MPI introduces additional process-launch,
communication, synchronization, and data-distribution overhead. This
overhead outweighs the benefit of the additional worker node.

As the dataset becomes larger, the computational work dominates the
distributed overhead. The two-node configuration becomes competitive at
approximately 4 GB and provides increasing benefits for larger datasets.

---

# 10. OpenMP vs MPI

| Dataset | Best OpenMP | Best 1-Node MPI | Best 2-Node MPI |
|---|---:|---:|---:|
| 160 MB | **0.084** | 0.636 | 0.976 |
| 500 MB | **0.230** | 0.880 | 1.186 |
| 1 GB | **0.446** | 1.144 | 1.374 |
| 2 GB | **0.806** | 1.628 | 1.744 |
| 4 GB | **1.626** | 2.544 | 2.502 |
| 8 GB | 5.258 | 4.442 | **3.994** |
| 10 GB | 6.136 | 5.406 | **4.780** |

The results show a transition in the most effective execution model.

For datasets up to approximately 4 GB, OpenMP provides the lowest measured
runtime.

For 8 GB and 10 GB, MPI becomes faster than OpenMP. At 10 GB, the two-node
MPI implementation achieves the lowest measured runtime.

---

# 11. Overall Speedup at 10 GB

The largest dataset provides the clearest comparison.

| Configuration | Runtime | Speedup vs Serial |
|---|---:|---:|
| Serial | 21.908 s | 1.00x |
| OpenMP — 12 threads | 6.136 s | 3.57x |
| Single-node MPI — 12 processes | 5.406 s | 4.05x |
| Two-node MPI — 12 processes | **4.780 s** | **4.58x** |

Therefore, for the 10 GB dataset:

- OpenMP reduces runtime by approximately 72.0%.
- Single-node MPI reduces runtime by approximately 75.3%.
- Two-node MPI reduces runtime by approximately 78.2%.

The two-node MPI configuration is approximately:

- 22.1% faster than OpenMP
- 11.6% faster than single-node MPI
- 4.58x faster than the serial baseline

---

# 12. Key Findings

## 12.1 OpenMP provides strong shared-memory performance

OpenMP substantially reduces runtime without requiring inter-node
communication. It provides the best performance for the smaller datasets.

## 12.2 MPI becomes more effective as workload increases

MPI overhead has a greater relative effect on small datasets. As the amount
of data increases, the parallel processing work becomes large enough to
justify the MPI overhead.

## 12.3 Distributed MPI has a workload-size crossover

The two-node configuration is slower than the best single-node MPI
configuration for datasets below approximately 4 GB.

At approximately 4 GB, the two configurations reach similar performance.

For 8 GB and 10 GB, the two-node configuration becomes faster.

## 12.4 Additional physical resources become useful for large datasets

The master node provides 8 physical CPU cores and the worker provides another
4 physical CPU cores.

The 12-process two-node configuration distributes these processes as:

    Master: 8 MPI processes
    Worker: 4 MPI processes

This provides access to 12 physical cores across the two machines.

## 12.5 Largest measured performance improvement

For the 10 GB dataset:

    Serial                  : 21.908 s
    OpenMP, 12 threads      : 6.136 s
    Single-node MPI, 12     : 5.406 s
    Two-node MPI, 12        : 4.780 s

The two-node MPI implementation achieves the best measured performance.

---

# 13. Conclusion

The experiments demonstrate that parallel performance depends strongly on
both workload size and execution architecture.

OpenMP provides highly effective shared-memory parallelism and is the best
measured approach for smaller datasets.

MPI becomes increasingly effective as the dataset size grows. Distributed
MPI initially suffers from the additional overhead of using multiple
machines, but this overhead becomes less significant for larger workloads.

The measured results show an approximate crossover around the 4 GB dataset.
Beyond this point, the two-node MPI configuration becomes increasingly
competitive and eventually provides the best performance.

For the largest tested dataset, 10 GB, two-node MPI achieves a runtime of
4.780 seconds, compared with 5.406 seconds for single-node MPI and
6.136 seconds for OpenMP.

This demonstrates that distributed-memory parallelism can provide measurable
benefits for sufficiently large log-analytics workloads, while
shared-memory OpenMP remains preferable when the workload is small enough
that distributed execution overhead dominates.
