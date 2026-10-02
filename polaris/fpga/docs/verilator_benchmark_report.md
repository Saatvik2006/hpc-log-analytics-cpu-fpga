# POLARIS FPGA Log Parser
## Verilator Simulation Benchmark Report

---

## 1. Objective

This benchmark evaluates the RTL simulation performance of the POLARIS HPC job-log parser using Verilator.

Five parallel parser architectures are evaluated:

- 1 parser
- 2 parsers
- 4 parsers
- 8 parsers
- 16 parsers

The purpose of this benchmark is to determine the number of FPGA clock cycles required to process the POLARIS log data for each architecture.

The measured `cycles/byte` values will subsequently be combined with the maximum operating frequency obtained from Vivado synthesis and implementation to estimate the theoretical FPGA processing time.

---

## 2. Parser Architecture

The POLARIS parser is implemented as a streaming ASCII/CSV parser.

The parser processes the input data byte by byte and identifies CSV fields while handling quoted fields and record boundaries.

The same parser architecture is replicated to create the parallel configurations.

| Architecture | Number of Parsers |
|---|---:|
| Single | 1 |
| Dual | 2 |
| Quad | 4 |
| Eight-way | 8 |
| Sixteen-way | 16 |

The benchmark uses the same parser functionality across all configurations.

---

## 3. Benchmark Datasets

The benchmark was performed using the following POLARIS datasets:

| Dataset |
|---|
| 160 MB |
| 500 MB |
| 1 GB |
| 2 GB |
| 4 GB |

The large datasets are used to evaluate steady-state behavior because the measured cycles/byte values remain highly consistent as the dataset size increases.

---

## 4. Benchmark Metrics

### 4.1 FPGA Cycles

The total number of simulated clock cycles required by the RTL design to process the input data.

### 4.2 Cycles per Byte

The primary RTL performance metric is:

\[
\text{Cycles/Byte} =
\frac{\text{FPGA Cycles}}
{\text{Data Bytes}}
\]

This metric is independent of the host computer used for Verilator simulation and will be used later for FPGA performance calculations.

### 4.3 Verilator Simulation Time

The wall-clock time required by Verilator to simulate the RTL design.

This represents host-side simulation performance and is **not FPGA execution time**.

### 4.4 Simulation Throughput

\[
\text{Simulation Throughput} =
\frac{\text{Data Bytes}}
{\text{Simulation Time}}
\]

This represents the rate at which the host computer executes the Verilator simulation.

### 4.5 Records per Second

\[
\text{Records/s} =
\frac{\text{Records Processed}}
{\text{Simulation Time}}
\]

---

# 5. Verilator Benchmark Results

## 5.1 Single-Parser Architecture

The single-parser implementation provides the baseline for comparison with the parallel architectures.

The measured steady-state behavior is approximately:

**1.00374 cycles/byte**

The cycles/byte value remains highly consistent across the large datasets.

---

## 5.2 Dual-Parser Architecture

| Dataset | Cycles/Byte | Simulation Throughput |
|---|---:|---:|
| 160 MB | 0.503739 | 11.2247 MB/s |
| 500 MB | 0.503738 | 12.5685 MB/s |
| 1 GB | 0.503736 | 12.5955 MB/s |
| 2 GB | 0.503735 | 12.5587 MB/s |
| 4 GB | 0.503735 | 12.5664 MB/s |

Steady-state result:

**~0.5037 cycles/byte**

---

## 5.3 Quad-Parser Architecture

| Dataset | Cycles/Byte | Simulation Throughput |
|---|---:|---:|
| 160 MB | 0.250947 | 12.5355 MB/s |
| 500 MB | 0.250936 | 12.5685 MB/s |
| 1 GB | 0.250936 | 12.5955 MB/s |
| 2 GB | 0.250935 | 12.5587 MB/s |
| 4 GB | 0.250935 | 12.5664 MB/s |

Steady-state result:

**~0.25094 cycles/byte**

---

## 5.4 Eight-Parser Architecture

| Dataset | Cycles/Byte | Simulation Throughput |
|---|---:|---:|
| 160 MB | 0.125488 | 21.4761 MB/s |
| 500 MB | 0.125472 | 21.4217 MB/s |
| 1 GB | 0.125469 | 21.4379 MB/s |
| 2 GB | 0.125468 | 21.3255 MB/s |
| 4 GB | 0.125468 | 21.4462 MB/s |

Steady-state result:

**~0.12547 cycles/byte**

---

## 5.5 Sixteen-Parser Architecture

| Dataset | Cycles/Byte | Simulation Throughput |
|---|---:|---:|
| 160 MB | 0.0627489 | 9.80246 MB/s |
| 500 MB | 0.0627412 | 9.78790 MB/s |
| 1 GB | 0.0627361 | 9.82259 MB/s |
| 2 GB | 0.0627343 | 9.81982 MB/s |
| 4 GB | 0.0627342 | 9.81813 MB/s |

Steady-state result:

**~0.062734 cycles/byte**

---

# 6. Steady-State Performance

The measured cycles/byte values for the parallel architectures are:

| Parser Count | Steady-State Cycles/Byte |
|---:|---:|
| 1 | ~1.00374 |
| 2 | ~0.50374 |
| 4 | ~0.25094 |
| 8 | ~0.12547 |
| 16 | ~0.062734 |

The results show that the cycles/byte approximately halve whenever the number of parser instances is doubled.

\[
1.00374
\rightarrow
0.50374
\rightarrow
0.25094
\rightarrow
0.12547
\rightarrow
0.062734
\]

This demonstrates the expected scaling behavior of the parallel parser datapath in the RTL simulation.

---

# 7. Scaling Analysis

The approximate relationship between parser count and cycles/byte is:

\[
\text{Cycles/Byte} \propto \frac{1}{N}
\]

where \(N\) is the number of parser instances.

The measured values are close to the ideal scaling relationship:

| Parser Count | Measured Cycles/Byte | Ideal Approximation |
|---:|---:|---:|
| 1 | ~1.00374 | 1.00000 |
| 2 | ~0.50374 | 0.50000 |
| 4 | ~0.25094 | 0.25000 |
| 8 | ~0.12547 | 0.12500 |
| 16 | ~0.062734 | 0.062500 |

The small differences from the ideal values are due to the architecture's control, partitioning, and record-handling behavior.

---

# 8. Simulation Throughput vs FPGA Throughput

The Verilator simulation throughput must not be interpreted as the actual throughput of the FPGA implementation.

Verilator executes the RTL model on the host CPU.

Therefore:

**Verilator simulation throughput**

represents the performance of the software simulator on the host system.

The actual FPGA performance will depend on:

- FPGA clock frequency
- RTL critical path
- Parser count
- Cycles per byte
- Input/data interface
- FPGA implementation resources

For this reason, the main metric carried forward from the Verilator benchmark is:

\[
\boxed{\text{Cycles/Byte}}
\]

rather than the host-side Verilator MB/s measurement.

---

# 9. FPGA Performance Calculation

After synthesis and implementation in Vivado, the maximum operating frequency \(F_{max}\) will be obtained for each parser architecture.

The theoretical FPGA processing time for a dataset is:

\[
T =
\frac{
\text{Data Bytes} \times \text{Cycles/Byte}
}{
F_{max}
}
\]

where:

- `Data Bytes` = number of bytes in the dataset
- `Cycles/Byte` = measured from Verilator RTL simulation
- `Fmax` = maximum operating frequency obtained from FPGA timing analysis

For example, for the 16-parser architecture:

\[
T_{16} =
\frac{
\text{Data Bytes} \times 0.062734
}{
F_{max,16}
}
\]

Similarly, the calculation will be performed for the 1-, 2-, 4-, and 8-parser architectures.

---

# 10. Final FPGA Benchmark

The final hardware benchmark will combine the Verilator cycles/byte results with the Vivado timing results.

| Parsers | Cycles/Byte | Fmax | 160 MB | 500 MB | 1 GB | 2 GB | 4 GB |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | ~1.00374 | — | — | — | — | — | — |
| 2 | ~0.50374 | — | — | — | — | — | — |
| 4 | ~0.25094 | — | — | — | — | — | — |
| 8 | ~0.12547 | — | — | — | — | — | — |
| 16 | ~0.062734 | — | — | — | — | — | — |

The `Fmax` values will be obtained from Vivado rather than assumed.

The processing times for the five dataset sizes will then be calculated using the measured cycles/byte and the corresponding FPGA frequency.

---

# 11. Benchmark Methodology Summary

The complete evaluation follows this sequence:

```text
POLARIS CSV Dataset
        |
        v
CSV / ASCII Byte Stream
        |
        v
+-----------------------+
| FPGA Parser           |
|                       |
| 1 / 2 / 4 / 8 / 16   |
| parallel instances    |
+-----------------------+
        |
        v
Verilator RTL Simulation
        |
        v
Cycles / Byte
        |
        v
Vivado Synthesis
        |
        v
Vivado Implementation
        |
        v
Maximum Operating Frequency
        |
        v
Theoretical FPGA Processing Time
