# NASA Hybrid MPI + OpenMP Benchmark

## 1. Scope

This document records the final NASA hybrid MPI + OpenMP benchmark.

The implementation performs the NASA log parsing workload together with
IP-level analytics.

Every configuration was executed five times. The reported execution time is
the arithmetic mean of the five runs. Sample standard deviation is reported
to show run-to-run variability.

---

## 2. Hardware and Execution Modes

### Master

- Intel Core i5-3570
- 4 physical cores
- 4 hardware threads

### Worker

- Intel Core i5-12450HX
- 8 physical cores
- 12 hardware threads

### Execution Modes

1. **Master-only** — all MPI processes execute on the master.
2. **Worker-only** — all MPI processes execute on the worker.
3. **Two-node** — MPI processes are distributed between master and worker.

---

## 3. Dataset Sizes

| Dataset | Size |
|---|---:|
| `access.log` | 161 MB |
| `nasa_500MB.log` | 500 MB |
| `nasa_1GB.log` | 1 GB |
| `nasa_2GB.log` | 2 GB |
| `nasa_4GB.log` | 4 GB |
| `nasa_8GB.log` | 8 GB |
| `nasa_10GB.log` | 10 GB |

---

## 4. Benchmark Methodology

Each configuration was executed five times.

The reported mean runtime is:

\[
T_{mean} = \frac{T_1+T_2+T_3+T_4+T_5}{5}
\]

The sample standard deviation is used to describe run-to-run variation.

### Speedup

Speedup is calculated relative to the NASA serial
baseline for the same dataset:

\[
S = \frac{T_{serial}}{T_{parallel}}
\]

where:

- `T_serial` = mean runtime of the serial implementation
- `T_parallel` = mean runtime of the selected parallel configuration

A speedup greater than 1 indicates that the parallel configuration is faster
than the serial baseline.

The best configuration is always selected using the **lowest measured mean
runtime**, not by MPI count or OpenMP thread count.

---

# 5. Serial Baseline

| Dataset | Mean Time (s) | Std. Dev. (s) |
|---|---:|---:|
| 161 MB | 0.684 | 0.032 |
| 500 MB | 2.070 | 0.081 |
| 1 GB | 4.402 | 0.030 |
| 2 GB | 8.706 | 0.048 |
| 4 GB | 16.744 | 0.116 |
| 8 GB | 33.388 | 0.182 |
| 10 GB | 43.738 | 0.500 |

---

# 6. 161 MB Benchmark

## 6.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | 0.588 | 0.018 | 1.163× |
| 2 MPI × 6 OMP | 0.798 | 0.046 | 0.857× |
| 3 MPI × 4 OMP | 0.642 | 0.016 | 1.065× |
| 4 MPI × 3 OMP | 0.678 | 0.021 | 1.009× |
| 6 MPI × 2 OMP | 0.772 | 0.008 | 0.886× |
| 8 MPI × 1 OMP | 0.826 | 0.037 | 0.828× |

**Best master-only:** 1 MPI × 12 OMP — **0.588 s**

## 6.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 0.600 | 0.008 | 1.140× |
| 2 MPI × 2 OMP | 0.852 | 0.010 | 0.803× |
| 4 MPI × 1 OMP | 0.606 | 0.005 | 1.129× |

**Best worker-only:** 1 MPI × 4 OMP — **0.600 s**

## 6.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 1.384 | 0.032 | 0.494× |
| 2 MPI × 2 OMP | 1.388 | 0.015 | 0.493× |
| 2 MPI × 4 OMP | 1.376 | 0.014 | 0.497× |
| 4 MPI × 1 OMP | 1.166 | 0.021 | 0.586× |
| 4 MPI × 2 OMP | 1.042 | 0.009 | 0.657× |
| 6 MPI × 1 OMP | 1.128 | 0.020 | 0.606× |
| 6 MPI × 2 OMP | 1.004 | 0.016 | 0.681× |
| 8 MPI × 1 OMP | 1.120 | 0.016 | 0.611× |
| 12 MPI × 1 OMP | 1.146 | 0.024 | 0.597× |

**Best two-node:** 6 MPI × 2 OMP — **1.004 s**

**Overall best:** Master-only, 1 MPI × 12 OMP — **0.588 s**, **1.163×**

---

# 7. 500 MB Benchmark

## 7.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | 0.882 | 0.040 | 2.347× |
| 2 MPI × 6 OMP | 1.410 | 0.029 | 1.468× |
| 3 MPI × 4 OMP | 0.974 | 0.034 | 2.125× |
| 4 MPI × 3 OMP | 0.966 | 0.029 | 2.143× |
| 6 MPI × 2 OMP | 1.060 | 0.025 | 1.953× |
| 8 MPI × 1 OMP | 1.102 | 0.054 | 1.878× |

**Best master-only:** 1 MPI × 12 OMP — **0.882 s**

## 7.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 1.214 | 0.013 | 1.705× |
| 2 MPI × 2 OMP | 1.960 | 0.008 | 1.056× |
| 4 MPI × 1 OMP | 1.236 | 0.030 | 1.675× |

**Best worker-only:** 1 MPI × 4 OMP — **1.214 s**

## 7.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 2.532 | 0.012 | 0.818× |
| 2 MPI × 2 OMP | 2.534 | 0.036 | 0.817× |
| 2 MPI × 4 OMP | 2.498 | 0.047 | 0.829× |
| 4 MPI × 1 OMP | 1.750 | 0.050 | 1.183× |
| 4 MPI × 2 OMP | 1.348 | 0.039 | 1.536× |
| 6 MPI × 1 OMP | 1.552 | 0.025 | 1.334× |
| 6 MPI × 2 OMP | 1.268 | 0.063 | 1.632× |
| 8 MPI × 1 OMP | 1.436 | 0.038 | 1.442× |
| 12 MPI × 1 OMP | 1.352 | 0.040 | 1.531× |

**Best two-node:** 6 MPI × 2 OMP — **1.268 s**

**Overall best:** Master-only, 1 MPI × 12 OMP — **0.882 s**, **2.347×**

---

# 8. 1 GB Benchmark

## 8.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | 1.212 | 0.047 | 3.632× |
| 2 MPI × 6 OMP | 2.302 | 0.038 | 1.912× |
| 3 MPI × 4 OMP | **1.204** | 0.017 | **3.656×** |
| 4 MPI × 3 OMP | 1.282 | 0.040 | 3.434× |
| 6 MPI × 2 OMP | 1.312 | 0.051 | 3.355× |
| 8 MPI × 1 OMP | 1.424 | 0.028 | 3.091× |

**Best master-only:** 3 MPI × 4 OMP — **1.204 s**

## 8.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 2.138 | 0.018 | 2.059× |
| 2 MPI × 2 OMP | 3.692 | 0.027 | 1.192× |
| 4 MPI × 1 OMP | 2.176 | 0.034 | 2.023× |

**Best worker-only:** 1 MPI × 4 OMP — **2.138 s**

## 8.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 4.240 | 0.036 | 1.038× |
| 2 MPI × 2 OMP | 4.228 | 0.016 | 1.041× |
| 2 MPI × 4 OMP | 4.240 | 0.041 | 1.038× |
| 4 MPI × 1 OMP | 2.656 | 0.021 | 1.658× |
| 4 MPI × 2 OMP | 1.840 | 0.012 | 2.392× |
| 6 MPI × 1 OMP | 2.116 | 0.044 | 2.080× |
| 6 MPI × 2 OMP | 1.578 | 0.012 | 2.790× |
| 8 MPI × 1 OMP | 1.900 | 0.036 | 2.317× |
| 12 MPI × 1 OMP | 1.674 | 0.067 | 2.630× |

**Best two-node:** 6 MPI × 2 OMP — **1.578 s**

**Overall best:** Master-only, 3 MPI × 4 OMP — **1.204 s**, **3.656×**

---

# 9. 2 GB Benchmark

## 9.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | 1.984 | 0.063 | 4.388× |
| 2 MPI × 6 OMP | 4.070 | 0.016 | 2.139× |
| 3 MPI × 4 OMP | 1.976 | 0.065 | 4.405× |
| 4 MPI × 3 OMP | **1.962** | 0.069 | **4.437×** |
| 6 MPI × 2 OMP | 2.108 | 0.079 | 4.130× |
| 8 MPI × 1 OMP | 2.254 | 0.026 | 3.861× |

**Best master-only:** 4 MPI × 3 OMP — **1.962 s**

## 9.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 4.004 | 0.048 | 2.174× |
| 2 MPI × 2 OMP | 7.040 | 0.008 | 1.237× |
| 4 MPI × 1 OMP | 4.030 | 0.061 | 2.160× |

**Best worker-only:** 1 MPI × 4 OMP — **4.004 s**

## 9.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 7.642 | 0.075 | 1.139× |
| 2 MPI × 2 OMP | 7.670 | 0.053 | 1.135× |
| 2 MPI × 4 OMP | 7.606 | 0.076 | 1.145× |
| 4 MPI × 1 OMP | 4.332 | 0.070 | 2.010× |
| 4 MPI × 2 OMP | 2.762 | 0.040 | 3.151× |
| 6 MPI × 1 OMP | 3.260 | 0.058 | 2.671× |
| 6 MPI × 2 OMP | 2.186 | 0.025 | 3.983× |
| 8 MPI × 1 OMP | 2.734 | 0.065 | 3.184× |
| 12 MPI × 1 OMP | 2.326 | 0.044 | 3.742× |

**Best two-node:** 12 MPI × 1 OMP — **2.326 s**

**Overall best:** Master-only, 4 MPI × 3 OMP — **1.962 s**, **4.437×**

---

# 10. 4 GB Benchmark

## 10.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | 3.186 | 0.071 | 5.255× |
| 2 MPI × 6 OMP | 7.588 | 0.054 | 2.207× |
| 3 MPI × 4 OMP | 3.148 | 0.035 | 5.319× |
| 4 MPI × 3 OMP | 3.046 | 0.300 | 5.498× |
| 6 MPI × 2 OMP | **2.960** | 0.088 | **5.657×** |
| 8 MPI × 1 OMP | 3.640 | 0.046 | 4.600× |

**Best master-only:** 6 MPI × 2 OMP — **2.960 s**

## 10.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 16.300 | 0.347 | 1.027× |
| 2 MPI × 2 OMP | 18.174 | 0.171 | 0.921× |
| 4 MPI × 1 OMP | 16.770 | 0.329 | 0.999× |

**Best worker-only:** 1 MPI × 4 OMP — **16.300 s**

## 10.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 14.550 | 0.089 | 1.151× |
| 2 MPI × 2 OMP | 14.332 | 0.116 | 1.168× |
| 2 MPI × 4 OMP | 14.302 | 0.131 | 1.171× |
| 4 MPI × 1 OMP | 7.678 | 0.028 | 2.181× |
| 4 MPI × 2 OMP | 4.572 | 0.022 | 3.662× |
| 6 MPI × 1 OMP | 5.460 | 0.046 | 3.067× |
| 6 MPI × 2 OMP | **3.392** | 0.036 | **4.936×** |
| 8 MPI × 1 OMP | 4.418 | 0.053 | 3.790× |
| 12 MPI × 1 OMP | 3.494 | 0.031 | 4.791× |

**Best two-node:** 6 MPI × 2 OMP — **3.392 s**

**Overall best:** Master-only, 6 MPI × 2 OMP — **2.960 s**, **5.657×**

---

# 11. 8 GB Benchmark

## 11.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | **4.436** | 0.139 | **7.529×** |
| 2 MPI × 6 OMP | 9.122 | 0.023 | 3.660× |
| 3 MPI × 4 OMP | 4.368 | 0.129 | 7.644× |
| 4 MPI × 3 OMP | 4.262 | 0.116 | 7.834× |
| 6 MPI × 2 OMP | 4.328 | 0.045 | 7.714× |
| 8 MPI × 1 OMP | 4.646 | 0.151 | 7.184× |

**Best master-only:** 4 MPI × 3 OMP — **4.262 s**

## 11.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 9.480 | 0.024 | 3.522× |
| 2 MPI × 2 OMP | 16.656 | 0.017 | 2.005× |
| 4 MPI × 1 OMP | 9.292 | 0.037 | 3.593× |

**Best worker-only:** 4 MPI × 1 OMP — **9.292 s**

## 11.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 17.046 | 0.009 | 1.958× |
| 2 MPI × 2 OMP | 17.062 | 0.091 | 1.957× |
| 2 MPI × 4 OMP | 17.252 | 0.357 | 1.935× |
| 4 MPI × 1 OMP | 9.118 | 0.016 | 3.662× |
| 4 MPI × 2 OMP | 5.386 | 0.026 | 6.198× |
| 6 MPI × 1 OMP | 6.396 | 0.040 | 5.220× |
| **6 MPI × 2 OMP** | **3.936** | 0.037 | **8.483×** |
| 8 MPI × 1 OMP | 5.118 | 0.048 | 6.524× |
| 12 MPI × 1 OMP | 4.006 | 0.039 | 8.339× |

**Best two-node:** 6 MPI × 2 OMP — **3.936 s**

**Overall best:** Two-node, 6 MPI × 2 OMP — **3.936 s**, **8.483×**

---

# 12. 10 GB Benchmark

## 12.1 Master-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 12 OMP | **6.840** | 0.045 | **6.394×** |
| 2 MPI × 6 OMP | 18.672 | 0.037 | 2.342× |
| 3 MPI × 4 OMP | 6.992 | 0.060 | 6.257× |
| 4 MPI × 3 OMP | 7.086 | 0.092 | 6.174× |
| 6 MPI × 2 OMP | 7.198 | 0.202 | 6.077× |
| 8 MPI × 1 OMP | 8.496 | 0.157 | 5.149× |

**Best master-only:** 1 MPI × 12 OMP — **6.840 s**

## 12.2 Worker-only

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 1 MPI × 4 OMP | 66.196 | 8.480 | 0.661× |
| 2 MPI × 2 OMP | 57.542 | 4.307 | 0.760× |
| 4 MPI × 1 OMP | **49.502** | 0.454 | **0.884×** |

**Best worker-only:** 4 MPI × 1 OMP — **49.502 s**

## 12.3 Two-node

| Configuration | Mean (s) | Std. Dev. (s) | Speedup |
|---|---:|---:|---:|
| 2 MPI × 1 OMP | 39.138 | 4.445 | 1.118× |
| 2 MPI × 2 OMP | 34.630 | 0.211 | 1.263× |
| 2 MPI × 4 OMP | 34.502 | 0.234 | 1.268× |
| 4 MPI × 1 OMP | 17.892 | 0.142 | 2.445× |
| 4 MPI × 2 OMP | 10.136 | 0.058 | 4.315× |
| 6 MPI × 1 OMP | 12.362 | 0.116 | 3.538× |
| **6 MPI × 2 OMP** | **7.092** | 0.042 | **6.167×** |
| 8 MPI × 1 OMP | 9.548 | 0.108 | 4.581× |
| 12 MPI × 1 OMP | 7.356 | 0.142 | 5.947× |

**Best two-node:** 6 MPI × 2 OMP — **7.092 s**

**Overall best:** Master-only, 1 MPI × 12 OMP — **6.840 s**, **6.394×**

---

# 13. Best Configuration by Dataset

| Dataset | Best Mode | Best Configuration | Runtime (s) | Speedup |
|---|---|---|---:|---:|
| 161 MB | Master-only | 1 MPI × 12 OMP | 0.588 | 1.163× |
| 500 MB | Master-only | 1 MPI × 12 OMP | 0.882 | 2.347× |
| 1 GB | Master-only | 3 MPI × 4 OMP | 1.204 | 3.656× |
| 2 GB | Master-only | 4 MPI × 3 OMP | 1.962 | 4.437× |
| 4 GB | Master-only | 6 MPI × 2 OMP | 2.960 | 5.657× |
| 8 GB | Two-node | 6 MPI × 2 OMP | 3.936 | 8.483× |
| 10 GB | Master-only | 1 MPI × 12 OMP | 6.840 | 6.394× |

---

# 14. Best Configuration by Execution Mode

| Dataset | Master-only Best | Worker-only Best | Two-node Best |
|---|---|---|---|
| 161 MB | 1×12 — 0.588 s | 1×4 — 0.600 s | 6×2 — 1.004 s |
| 500 MB | 1×12 — 0.882 s | 1×4 — 1.214 s | 6×2 — 1.268 s |
| 1 GB | 3×4 — 1.204 s | 1×4 — 2.138 s | 6×2 — 1.578 s |
| 2 GB | 4×3 — 1.962 s | 1×4 — 4.004 s | 12×1 — 2.326 s |
| 4 GB | 6×2 — 2.960 s | 1×4 — 16.300 s | 6×2 — 3.392 s |
| 8 GB | 4×3 — 4.262 s | 4×1 — 9.292 s | 6×2 — 3.936 s |
| 10 GB | 1×12 — 6.840 s | 4×1 — 49.502 s | 6×2 — 7.092 s |

---

# 15. Cross-Dataset Speedup

| Dataset | Serial (s) | Best Parallel (s) | Best Speedup | Best Configuration |
|---|---:|---:|---:|---|
| 161 MB | 0.684 | 0.588 | 1.163× | Master 1×12 |
| 500 MB | 2.070 | 0.882 | 2.347× | Master 1×12 |
| 1 GB | 4.402 | 1.204 | 3.656× | Master 3×4 |
| 2 GB | 8.706 | 1.962 | 4.437× | Master 4×3 |
| 4 GB | 16.744 | 2.960 | 5.657× | Master 6×2 |
| 8 GB | 33.388 | 3.936 | **8.483×** | Two-node 6×2 |
| 10 GB | 43.738 | 6.840 | 6.394× | Master 1×12 |

---

# 16. Important Performance Observations

### 16.1 No single configuration is universally optimal

The optimal MPI/OpenMP configuration changes with dataset size.

The best configuration progresses from:

- 1 MPI × 12 OMP for 161 MB and 500 MB
- 3 MPI × 4 OMP for 1 GB
- 4 MPI × 3 OMP for 2 GB
- 6 MPI × 2 OMP for 4 GB
- 6 MPI × 2 OMP across two nodes for 8 GB
- 1 MPI × 12 OMP for 10 GB

Therefore, the benchmark does not support the assumption that increasing
MPI processes or OpenMP threads monotonically improves performance.

### 16.2 Two-node execution becomes important at larger workloads

For 8 GB, the best overall result is:

**6 MPI × 2 OMP across two nodes = 3.936 s**

This produces an **8.483× speedup** over the 33.388 s serial baseline.

For 10 GB, the best two-node configuration is:

**6 MPI × 2 OMP = 7.092 s**

This produces a **6.167× speedup** over the 43.738 s serial baseline.

### 16.3 MPI-only increases are not always beneficial

For example, on the 10 GB master:

- 1 MPI × 12 OMP → 6.840 s
- 3 MPI × 4 OMP → 6.992 s
- 6 MPI × 2 OMP → 7.198 s
- 8 MPI × 1 OMP → 8.496 s

Increasing MPI processes while reducing OpenMP parallelism therefore
degrades performance in this case.

### 16.4 Hybrid balance matters

The two-node results demonstrate that combining MPI and OpenMP can be
substantially better than using MPI processes alone.

For 8 GB:

- 6 MPI × 1 OMP → 6.396 s
- 6 MPI × 2 OMP → **3.936 s**

For 10 GB:

- 6 MPI × 1 OMP → 12.362 s
- 6 MPI × 2 OMP → **7.092 s**

Thus, the additional OpenMP parallelism within each MPI process can provide
a substantial reduction in runtime.

---

# 17. Reproducibility

The following procedure was used throughout the benchmark:

1. Use the same NASA executable.
2. Use the same dataset for all configurations within a dataset size.
3. Execute every configuration five times.
4. Measure wall-clock execution time using `/usr/bin/time`.
5. Calculate the arithmetic mean.
6. Calculate sample standard deviation.
7. Compare each configuration against the serial
   baseline for the same dataset.
8. Select the configuration with the minimum mean runtime as the best
   configuration.

The datasets were accessed through the mounted external storage location
used by the project.

---

# 18. Final Results

The NASA hybrid implementation achieves its highest measured speedup of:

**8.483×**

for the:

**8 GB dataset using 6 MPI × 2 OpenMP across two nodes.**

The 10 GB dataset achieves:

**6.394×**

using:

**1 MPI × 12 OpenMP on the master.**

The results demonstrate that hybrid parallel performance depends on both
dataset size and the balance between distributed MPI processes and
shared-memory OpenMP workers.

The experiments therefore support evaluating the MPI/OpenMP configuration
space rather than assuming that maximum MPI or maximum OpenMP parallelism
will always produce the best performance.

---

# 19. Raw Benchmark Data

The original five-run measurements are retained separately from the
calculated means and speedups. The reported statistics in this document
were calculated directly from those five measurements.
