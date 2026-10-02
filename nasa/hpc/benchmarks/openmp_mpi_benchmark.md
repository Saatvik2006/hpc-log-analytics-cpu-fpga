# NASA Log Analytics — Serial, OpenMP and MPI Benchmark

## 1. Overview

This document presents the performance evaluation of the NASA log analytics implementation using three execution models:

1. Serial execution
2. OpenMP shared-memory parallel execution
3. MPI distributed-memory parallel execution

The experiments were performed on seven dataset sizes:

- 161 MB
- 500 MB
- 1 GB
- 2 GB
- 4 GB
- 8 GB
- 10 GB

Each configuration was executed five times. Reported runtimes are the arithmetic mean of the five runs.

Speedup is calculated relative to the corresponding serial baseline:

\[
S = \frac{T_{serial}}{T_{parallel}}
\]

---

# 2. Hardware Configuration

## Master

| Resource | Configuration |
|---|---:|
| Physical CPU cores | 8 |
| Hardware threads | 12 |
| OpenMP configurations | 1, 2, 3, 4, 6, 8, 12 |
| MPI configurations | 1, 2, 3, 4, 6, 8 |

## Worker

| Resource | Configuration |
|---|---:|
| Physical CPU cores | 4 |
| Hardware threads | 4 |
| OpenMP configurations | 1, 2, 3, 4 |
| MPI configurations | 1, 2, 3, 4 |

## Two-node MPI

The distributed MPI experiments used:

| MPI processes | Master | Worker |
|---:|---:|---:|
| 2 | 1 | 1 |
| 4 | 2 | 2 |
| 6 | 4 | 2 |
| 8 | 6 | 2 |
| 12 | 8 | 4 |

The benchmark was intentionally restricted to configurations appropriate for the available CPU resources.

---

# 3. Dataset Configuration

| Dataset | Size |
|---|---:|
| `access.log` | 161 MB |
| `nasa_500MB.log` | 500 MB |
| `nasa_1GB.log` | 1 GB |
| `nasa_2GB.log` | 2 GB |
| `nasa_4GB.log` | 4 GB |
| `nasa_8GB.log` | 8 GB |
| `nasa_10GB.log` | 10 GB |

All implementations operated on the same corresponding dataset for a given experiment.

---

# 4. Serial Baseline

The serial implementation was executed five times for each dataset.

| Dataset | Mean Runtime (s) |
|---|---:|
| 161 MB | 0.684 |
| 500 MB | 2.070 |
| 1 GB | 4.402 |
| 2 GB | 8.706 |
| 4 GB | 16.744 |
| 8 GB | 33.388 |
| 10 GB | 43.738 |

These values are used as the reference for all speedup calculations.

---

# 5. OpenMP-Only Benchmark

## 5.1 Master OpenMP

The master system was tested using 1, 2, 3, 4, 6, 8 and 12 OpenMP threads.

### Mean Runtime

| Dataset | 1 OMP | 2 OMP | 3 OMP | 4 OMP | 6 OMP | 8 OMP | 12 OMP |
|---|---:|---:|---:|---:|---:|---:|---:|
| 161 MB | 0.704 | 0.368 | 0.260 | 0.196 | 0.184 | 0.136 | 0.138 |
| 500 MB | 2.106 | 1.118 | 0.770 | 0.594 | 0.506 | 0.396 | 0.364 |
| 1 GB | 4.530 | 2.370 | 1.580 | 1.226 | 1.038 | 0.814 | 0.710 |
| 2 GB | 8.964 | 4.726 | 3.152 | 2.404 | 2.002 | 1.636 | 1.430 |
| 4 GB | 17.190 | 9.176 | 6.236 | 4.762 | 3.940 | 3.122 | 2.662 |
| 8 GB | 34.468 | 18.286 | 12.466 | 9.518 | 8.018 | 6.118 | 5.346 |
| 10 GB | 44.980 | 23.458 | 15.846 | 12.252 | 9.946 | 7.920 | 7.172 |

### Best Master OpenMP Configuration

| Dataset | Best Configuration | Runtime (s) | Speedup |
|---|---:|---:|---:|
| 161 MB | 8 OMP | 0.136 | 5.029× |
| 500 MB | 12 OMP | 0.364 | 5.687× |
| 1 GB | 12 OMP | 0.710 | 6.200× |
| 2 GB | 12 OMP | 1.430 | 6.088× |
| 4 GB | 12 OMP | 2.662 | 6.290× |
| 8 GB | 12 OMP | 5.346 | 6.245× |
| 10 GB | 12 OMP | 7.172 | 6.098× |

The master provides strong OpenMP scaling, with the 12-thread configuration producing approximately 6× speedup for datasets from 1 GB onward.

---

## 5.2 Worker OpenMP

The worker system was tested using 1, 2, 3 and 4 OpenMP threads.

### Mean Runtime

| Dataset | 1 OMP | 2 OMP | 3 OMP | 4 OMP |
|---|---:|---:|---:|---:|
| 161 MB | 1.174 | 0.592 | 0.414 | 0.326 |
| 500 MB | 3.716 | 1.818 | 1.256 | 0.994 |
| 1 GB | 7.428 | 3.668 | 2.526 | 2.016 |
| 2 GB | 14.674 | 7.242 | 4.988 | 3.954 |
| 4 GB | 29.192 | 14.630 | 9.936 | 7.924 |
| 8 GB | 63.202 | 32.914 | 25.604 | 20.128 |
| 10 GB | 106.650 | 53.198 | 41.790 | 32.104 |

### Best Worker OpenMP Configuration

| Dataset | Best Configuration | Runtime (s) | Speedup |
|---|---:|---:|---:|
| 161 MB | 4 OMP | 0.326 | 2.098× |
| 500 MB | 4 OMP | 0.994 | 2.082× |
| 1 GB | 4 OMP | 2.016 | 2.184× |
| 2 GB | 4 OMP | 3.954 | 2.202× |
| 4 GB | 4 OMP | 7.924 | 2.113× |
| 8 GB | 4 OMP | 20.128 | 1.659× |
| 10 GB | 4 OMP | 32.104 | 1.362× |

The worker reaches its best performance consistently at 4 OpenMP threads, corresponding to its four physical CPU cores.

---

# 6. MPI-Only Benchmark

## 6.1 Master MPI

### Mean Runtime

| Dataset | 1 MPI | 2 MPI | 3 MPI | 4 MPI | 6 MPI | 8 MPI |
|---|---:|---:|---:|---:|---:|---:|
| 161 MB | 1.288 | 0.910 | 0.818 | 0.748 | 0.794 | 0.800 |
| 500 MB | 2.780 | 1.692 | 1.320 | 1.194 | 1.222 | 1.076 |
| 1 GB | 5.196 | 2.904 | 2.176 | 1.766 | 1.826 | 1.498 |
| 2 GB | 9.760 | 5.236 | 3.794 | 3.012 | 2.952 | 2.412 |
| 4 GB | 18.944 | 9.860 | 6.864 | 5.370 | 5.142 | 4.090 |
| 8 GB | 37.706 | 19.100 | 13.104 | 10.094 | 9.430 | 7.318 |
| 10 GB | 48.454 | 25.236 | 17.492 | 13.534 | 11.544 | 9.380 |

### Best Master MPI Configuration

| Dataset | Best Configuration | Runtime (s) | Speedup |
|---|---:|---:|---:|
| 161 MB | 4 MPI | 0.748 | 0.914× |
| 500 MB | 8 MPI | 1.076 | 1.924× |
| 1 GB | 8 MPI | 1.498 | 2.939× |
| 2 GB | 8 MPI | 2.412 | 3.609× |
| 4 GB | 8 MPI | 4.090 | 4.094× |
| 8 GB | 8 MPI | 7.318 | 4.562× |
| 10 GB | 8 MPI | 9.380 | 4.663× |

The 161 MB dataset does not benefit from MPI on the master relative to the serial baseline because process-management and communication overhead become significant relative to the small workload.

---

## 6.2 Worker MPI

### Mean Runtime

| Dataset | 1 MPI | 2 MPI | 3 MPI | 4 MPI |
|---|---:|---:|---:|---:|
| 161 MB | 1.532 | 0.928 | 0.748 | 0.664 |
| 500 MB | 3.894 | 2.168 | 1.602 | 1.344 |
| 1 GB | 7.610 | 4.004 | 2.894 | 2.368 |
| 2 GB | 14.948 | 7.724 | 5.464 | 4.376 |
| 4 GB | 29.440 | 15.074 | 10.528 | 8.368 |
| 8 GB | 64.712 | 34.190 | 25.720 | 20.216 |
| 10 GB | 107.622 | 54.212 | 42.732 | 33.266 |

### Best Worker MPI Configuration

| Dataset | Best Configuration | Runtime (s) | Speedup |
|---|---:|---:|---:|
| 161 MB | 4 MPI | 0.664 | 1.030× |
| 500 MB | 4 MPI | 1.344 | 1.540× |
| 1 GB | 4 MPI | 2.368 | 1.859× |
| 2 GB | 4 MPI | 4.376 | 1.989× |
| 4 GB | 4 MPI | 8.368 | 2.001× |
| 8 GB | 4 MPI | 20.216 | 1.652× |
| 10 GB | 4 MPI | 33.266 | 1.315× |

The worker's best MPI configuration is consistently 4 MPI processes, matching its four physical cores.

---

# 7. Two-Node MPI Benchmark

MPI was distributed between the master and worker using the following process distributions:

- 2 MPI: 1 + 1
- 4 MPI: 2 + 2
- 6 MPI: 4 + 2
- 8 MPI: 6 + 2
- 12 MPI: 8 + 4

### Mean Runtime

| Dataset | 2 MPI | 4 MPI | 6 MPI | 8 MPI | 12 MPI |
|---|---:|---:|---:|---:|---:|
| 161 MB | 1.588 | 1.358 | 1.238 | 1.210 | 1.216 |
| 500 MB | 2.910 | 2.250 | 1.870 | 1.680 | 1.678 |
| 1 GB | 4.778 | 3.300 | 2.738 | 2.396 | 2.306 |
| 2 GB | 8.416 | 5.156 | 3.958 | 3.398 | 3.412 |
| 4 GB | 15.730 | 8.804 | 6.436 | 5.312 | 4.776 |
| 8 GB | 30.692 | 16.240 | 11.320 | 8.902 | 7.406 |
| 10 GB | 39.782 | 19.984 | 13.766 | 10.780 | 8.826 |

### Best Two-Node MPI Configuration

| Dataset | Best Configuration | Runtime (s) | Speedup |
|---|---:|---:|---:|
| 161 MB | 8 MPI | 1.210 | 0.565× |
| 500 MB | 12 MPI | 1.678 | 1.234× |
| 1 GB | 12 MPI | 2.306 | 1.909× |
| 2 GB | 8 MPI | 3.398 | 2.562× |
| 4 GB | 12 MPI | 4.776 | 3.506× |
| 8 GB | 12 MPI | 7.406 | 4.509× |
| 10 GB | 12 MPI | 8.826 | 4.958× |

For the smaller datasets, distributed MPI is slower than the serial execution because the workload is too small to amortize inter-process and inter-node overhead. As the dataset size increases, distributed MPI becomes increasingly beneficial.

---

# 8. Overall OpenMP and MPI Comparison

The following table selects the fastest measured configuration across the OpenMP and MPI experiments.

| Dataset | Serial (s) | Best OpenMP | Runtime (s) | Speedup | Best MPI | Runtime (s) | Speedup |
|---|---:|---|---:|---:|---|---:|---:|
| 161 MB | 0.684 | Master 8 OMP | 0.136 | 5.029× | Worker 4 MPI | 0.664 | 1.030× |
| 500 MB | 2.070 | Master 12 OMP | 0.364 | 5.687× | Master 8 MPI | 1.076 | 1.924× |
| 1 GB | 4.402 | Master 12 OMP | 0.710 | 6.200× | Master 8 MPI | 1.498 | 2.939× |
| 2 GB | 8.706 | Master 12 OMP | 1.430 | 6.088× | Master 8 MPI | 2.412 | 3.609× |
| 4 GB | 16.744 | Master 12 OMP | 2.662 | 6.290× | Master 8 MPI | 4.090 | 4.094× |
| 8 GB | 33.388 | Master 12 OMP | 5.346 | 6.245× | Master 8 MPI | 7.318 | 4.562× |
| 10 GB | 43.738 | Master 12 OMP | 7.172 | 6.098× | Two-node 12 MPI | 8.826 | 4.958× |

---

# 9. Best Configuration Across OpenMP and MPI

| Dataset | Best Overall Configuration | Runtime (s) | Speedup |
|---|---|---:|---:|
| 161 MB | Master — 8 OMP | 0.136 | 5.029× |
| 500 MB | Master — 12 OMP | 0.364 | 5.687× |
| 1 GB | Master — 12 OMP | 0.710 | 6.200× |
| 2 GB | Master — 12 OMP | 1.430 | 6.088× |
| 4 GB | Master — 12 OMP | 2.662 | 6.290× |
| 8 GB | Master — 12 OMP | 5.346 | 6.245× |
| 10 GB | Master — 12 OMP | 7.172 | 6.098× |

Within the OpenMP and MPI experiments, the master OpenMP implementation provides the best measured runtime for all datasets.

The largest measured speedup in this benchmark is **6.290×** for the 4 GB dataset using 12 OpenMP threads on the master.

---

# 10. Performance Analysis

## 10.1 Dataset Size

The serial runtime increases approximately with dataset size, confirming that the workload is predominantly data-processing and parsing bound.

The serial baseline increases from:

- 0.684 s for 161 MB
- 2.070 s for 500 MB
- 4.402 s for 1 GB
- 8.706 s for 2 GB
- 16.744 s for 4 GB
- 33.388 s for 8 GB
- 43.738 s for 10 GB

This provides a consistent baseline for evaluating parallel execution.

## 10.2 OpenMP Scaling

OpenMP provides the strongest performance on the master system.

The master reaches approximately 6× speedup for datasets from 1 GB onward. Increasing the number of threads from 1 to 12 substantially reduces execution time.

For example, for the 10 GB dataset:

- 1 OMP: 44.980 s
- 4 OMP: 12.252 s
- 8 OMP: 7.920 s
- 12 OMP: 7.172 s

The improvement becomes smaller as the number of threads increases, indicating that the workload eventually encounters limits from memory bandwidth, synchronization, parsing overhead, and other shared resources.

## 10.3 Worker Scaling

The worker also benefits from OpenMP and MPI, but its lower core count limits its absolute performance.

The 10 GB OpenMP runtime decreases from 106.650 s with one thread to 32.104 s with four threads.

The resulting speedup is approximately 1.36× relative to the serial baseline.

## 10.4 MPI Scaling

MPI shows progressively better scaling as the dataset becomes larger.

For the master, the 10 GB dataset decreases from:

- 48.454 s with 1 MPI process
- 25.236 s with 2 MPI processes
- 17.492 s with 3 MPI processes
- 13.534 s with 4 MPI processes
- 11.544 s with 6 MPI processes
- 9.380 s with 8 MPI processes

The corresponding best master MPI speedup is approximately 4.66× relative to the serial baseline.

## 10.5 Two-Node MPI

Two-node MPI becomes increasingly useful for large datasets.

For 10 GB:

- 2 MPI: 39.782 s
- 4 MPI: 19.984 s
- 6 MPI: 13.766 s
- 8 MPI: 10.780 s
- 12 MPI: 8.826 s

The best two-node MPI configuration achieves approximately **4.96× speedup** over serial execution.

For small datasets, however, the two-node configuration is slower than local execution because communication and distributed-process startup overhead dominate the relatively small computational workload.

---

# 11. Key Findings

1. **OpenMP provides the strongest overall performance** among the tested execution models.

2. The **master system consistently outperforms the worker**, which is consistent with its larger number of physical cores and hardware threads.

3. The master reaches approximately **6× speedup** with 12 OpenMP threads on datasets from 1 GB through 10 GB.

4. MPI scaling becomes increasingly effective as dataset size increases.

5. **Two-node MPI becomes competitive for large datasets**, reaching 4.958× speedup for the 10 GB dataset.

6. Small datasets are dominated more strongly by parallelization and communication overhead.

7. The best configuration depends on dataset size, but among the tested OpenMP and MPI configurations, **master-side OpenMP provides the lowest runtime for every dataset**.

8. The results demonstrate that increasing parallelism does not guarantee proportional speedup. Beyond a certain point, shared-memory contention, memory bandwidth, process-management overhead, and communication costs limit further gains.

---

# 12. Summary Table

| Dataset | Serial | Best OpenMP | OMP Speedup | Best MPI | MPI Speedup | Best Overall |
|---|---:|---|---:|---|---:|---|
| 161 MB | 0.684 s | Master 8 OMP | 5.029× | Worker 4 MPI | 1.030× | Master 8 OMP |
| 500 MB | 2.070 s | Master 12 OMP | 5.687× | Master 8 MPI | 1.924× | Master 12 OMP |
| 1 GB | 4.402 s | Master 12 OMP | 6.200× | Master 8 MPI | 2.939× | Master 12 OMP |
| 2 GB | 8.706 s | Master 12 OMP | 6.088× | Master 8 MPI | 3.609× | Master 12 OMP |
| 4 GB | 16.744 s | Master 12 OMP | 6.290× | Master 8 MPI | 4.094× | Master 12 OMP |
| 8 GB | 33.388 s | Master 12 OMP | 6.245× | Master 8 MPI | 4.562× | Master 12 OMP |
| 10 GB | 43.738 s | Master 12 OMP | 6.098× | Two-node 12 MPI | 4.958× | Master 12 OMP |

---

# 13. Conclusion

The NASA benchmark demonstrates clear benefits from parallel log processing.

OpenMP provides the strongest performance for the tested workload, particularly on the master system where 12 hardware threads are available. The best OpenMP configurations achieve approximately 5–6× speedup over the serial implementation.

MPI provides a second scalable execution path. Its benefits become more pronounced as the dataset size increases, with the two-node configuration achieving nearly 5× speedup on the 10 GB dataset.

The results also demonstrate the importance of workload size and hardware topology. Parallel execution introduces overhead, and therefore small datasets may not benefit from additional processes or distributed execution. Larger datasets provide sufficient computational work to amortize these costs and expose the scalability of the parallel implementations.

Overall, the benchmark establishes the serial, shared-memory OpenMP, and distributed-memory MPI performance characteristics of the NASA log analytics implementation across dataset sizes ranging from 161 MB to 10 GB.
