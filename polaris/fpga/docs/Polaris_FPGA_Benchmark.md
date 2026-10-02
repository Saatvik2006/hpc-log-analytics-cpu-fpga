# POLARIS FPGA Log Parser
## Verilator Simulation + Vivado FPGA Benchmark

---

## 1. Objective

This benchmark evaluates the performance and hardware scalability of the POLARIS HPC job-log parser using:

1. Verilator RTL simulation
2. Xilinx Vivado synthesis
3. Xilinx Vivado implementation and timing analysis

Three parser architectures are included in the final FPGA benchmark:

- 1 parser
- 2 parsers
- 4 parsers

The 8-parser and 16-parser architectures were also developed and simulated using Verilator, but could not be implemented on the selected FPGA because of LUT resource limitations.

The Verilator results provide the measured `cycles/byte`, while Vivado provides the maximum tested operating frequency and FPGA resource utilization.

These measurements are combined to calculate the theoretical FPGA processing time for each POLARIS dataset.

---

## 2. Target FPGA

The FPGA used for the Vivado synthesis and implementation is:

Device: xc7a35tcsg324-1
Family: Artix-7
Speed Grade: -1
Available LUTs: 20,800
Available Flip-Flops: 41,600

All three implemented architectures were evaluated on the same FPGA device.

---

## 3. Parser Architecture

The POLARIS parser is implemented as a streaming ASCII/CSV parser.

The parser processes the input data byte by byte and identifies CSV fields while handling quoted fields and record boundaries.

Parallel architectures are created by replicating the same `polaris_log_parser` module.

| Architecture | Parser Instances |
|---|---:|
| Single | 1 |
| Dual | 2 |
| Quad | 4 |
| Eight-way | 8 |
| Sixteen-way | 16 |

---

## 4. Benchmark Datasets

The benchmark uses the following POLARIS datasets:

| Dataset | Data Bytes |
|---|---:|
| 160 MB | 167,772,487 |
| 500 MB | 524,287,067 |
| 1 GB | 1,073,741,371 |
| 2 GB | 2,147,482,687 |
| 4 GB | 4,294,966,463 |

The large datasets were used to evaluate steady-state behavior.

---

## 5. Verilator Simulation Methodology

Verilator was used to simulate the RTL implementations.

The primary metric used for the FPGA performance calculation is:

\[
\text{Cycles/Byte}
=
\frac{\text{FPGA Cycles}}
{\text{Data Bytes}}
\]

The Verilator wall-clock simulation throughput is not considered FPGA hardware throughput.

Verilator executes the RTL model on the host CPU, whereas the FPGA will execute the synthesized hardware at its implemented clock frequency.

Therefore, the measured `cycles/byte` is carried forward from the Verilator benchmark and combined with the Vivado operating frequency.

---

## 6. Verilator Results

### 6.1 Single-Parser Architecture

The single-parser architecture provides the baseline.

Measured steady-state result:

Cycles/Byte ≈ 1.00374

The value remained highly consistent across the large datasets.

---

### 6.2 Dual-Parser Architecture

| Dataset | Cycles/Byte | Simulation Throughput |
|---|---:|---:|
| 160 MB | 0.503739 | 11.2247 MB/s |
| 500 MB | 0.503738 | 12.5685 MB/s |
| 1 GB | 0.503736 | 12.5955 MB/s |
| 2 GB | 0.503735 | 12.5587 MB/s |
| 4 GB | 0.503735 | 12.5664 MB/s |

Steady-state value used for FPGA calculations:

0.503735 cycles/byte

---

### 6.3 Quad-Parser Architecture

| Dataset | Cycles/Byte | Simulation Throughput |
|---|---:|---:|
| 160 MB | 0.250947 | 12.5355 MB/s |
| 500 MB | 0.250936 | 12.5685 MB/s |
| 1 GB | 0.250936 | 12.5955 MB/s |
| 2 GB | 0.250935 | 12.5587 MB/s |
| 4 GB | 0.250935 | 12.5664 MB/s |

Steady-state value used for FPGA calculations:

0.250935 cycles/byte

---

### 6.4 Eight-Parser Architecture

The eight-parser architecture was successfully simulated using Verilator.

Measured steady-state result:

0.12547 cycles/byte

However, the complete eight-parser RTL implementation could not be implemented on the target FPGA.

Vivado reported:

LUTs required  : 31,196
LUTs available : 20,800

Therefore:

\[
31,196 > 20,800
\]

The design exceeds the available LUT resources of the `xc7a35tcsg324-1`.

Vivado therefore stops implementation because the design cannot physically fit on the selected FPGA.

Consequently, no valid FPGA Fmax or FPGA processing time is reported for the eight-parser architecture.

---

### 6.5 Sixteen-Parser Architecture

The sixteen-parser architecture was also successfully simulated using Verilator.

Measured steady-state result:

0.062734 cycles/byte

However, the complete sixteen-parser implementation exceeds the available resources of the selected FPGA.

Since the eight-parser design already requires 31,196 LUTs while the device provides only 20,800 LUTs, the sixteen-parser architecture cannot fit on the same device.

Therefore, no FPGA Fmax or FPGA processing time is reported for the sixteen-parser architecture on the `xc7a35tcsg324-1`.

---

## 7. Steady-State Verilator Results

| Parser Count | Steady-State Cycles/Byte |
|---:|---:|
| 1 | ~1.00374 |
| 2 | ~0.503735 |
| 4 | ~0.250935 |
| 8 | ~0.12547 |
| 16 | ~0.062734 |

The results demonstrate approximately inverse scaling of cycles/byte with the number of parser instances.

\[
\text{Cycles/Byte}\propto\frac{1}{N}
\]

where \(N\) is the number of parser instances.

---

## 8. Vivado Timing Results

Vivado synthesis and implementation were performed for the complete 1-, 2-, and 4-parser architectures.

The following are the highest tested clock frequencies that successfully met timing.

| Architecture | Clock Period | Frequency | Timing Result |
|---|---:|---:|---|
| 1 parser | 8.100 ns | 123.457 MHz | Passed |
| 2 parsers | 8.300 ns | 120.482 MHz | Passed |
| 4 parsers | 9.050 ns | 110.497 MHz | Passed |

Frequency is calculated as:

\[
F=\frac{1}{T}
\]

These values represent the highest tested passing frequencies, rather than an infinitely fine-grained determination of the exact timing boundary.

---

## 9. Vivado Resource Utilization

Post-implementation utilization was obtained from Vivado for the three implemented architectures.

### 9.1 Single Parser

| Resource | Utilization |
|---|---:|
| LUT | 16% |
| Flip-Flop | 11% |
| IO | 1% |
| BUFG | 3% |

Timing:

WNS = +0.007 ns
TNS = 0 ns
Failed Endpoints = 0

The implementation successfully met the 8.100 ns clock constraint.

---

### 9.2 Dual Parser

| Resource | Utilization |
|---|---:|
| LUT | 33% |
| Flip-Flop | 22% |
| IO | 1% |
| BUFG | 3% |

Timing:

WNS = +0.066 ns
TNS = 0 ns
Failed Endpoints = 0

The implementation successfully met the 8.300 ns clock constraint.

---

### 9.3 Quad Parser

| Resource | Utilization |
|---|---:|
| LUT | 69% |
| Flip-Flop | 43% |
| IO | 1% |
| BUFG | 3% |

Timing:

WNS = +0.133 ns
TNS = 0 ns
Failed Endpoints = 0

The implementation successfully met the 9.050 ns clock constraint.

---

## 10. FPGA Processing-Time Calculation

The theoretical FPGA processing time is calculated using:

\[
T=
\frac{
\text{Data Bytes}\times\text{Cycles/Byte}
}{
F_{max}
}
\]

where:

- `Data Bytes` = number of data bytes in the POLARIS dataset
- `Cycles/Byte` = measured from Verilator
- `Fmax` = highest tested passing Vivado frequency

---

## 11. Single-Parser FPGA Processing Time

Parameters:

Cycles/Byte = 1.00374
Frequency   = 123.457 MHz

| Dataset | Theoretical FPGA Processing Time |
|---|---:|
| 160 MB | 1.36404 s |
| 500 MB | 4.26261 s |
| 1 GB | 8.72983 s |
| 2 GB | 17.45967 s |
| 4 GB | 34.91934 s |

---

## 12. Dual-Parser FPGA Processing Time

Parameters:

Cycles/Byte = 0.503735
Frequency   = 120.482 MHz

| Dataset | Theoretical FPGA Processing Time |
|---|---:|
| 160 MB | 0.70146 s |
| 500 MB | 2.19204 s |
| 1 GB | 4.48931 s |
| 2 GB | 8.97863 s |
| 4 GB | 17.95726 s |

---

## 13. Quad-Parser FPGA Processing Time

Parameters:

Cycles/Byte = 0.250935
Frequency   = 110.497 MHz

| Dataset | Theoretical FPGA Processing Time |
|---|---:|
| 160 MB | 0.38100 s |
| 500 MB | 1.19064 s |
| 1 GB | 2.43843 s |
| 2 GB | 4.87685 s |
| 4 GB | 9.75370 s |

---

## 14. Final FPGA Benchmark

The current complete FPGA benchmark is:

| Parsers | Cycles/Byte | Fmax | LUT | FF | 160 MB | 500 MB | 1 GB | 2 GB | 4 GB |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1.00374 | 123.457 MHz | 16% | 11% | 1.36404 s | 4.26261 s | 8.72983 s | 17.45967 s | 34.91934 s |
| 2 | 0.503735 | 120.482 MHz | 33% | 22% | 0.70146 s | 2.19204 s | 4.48931 s | 8.97863 s | 17.95726 s |
| 4 | 0.250935 | 110.497 MHz | 69% | 43% | 0.38100 s | 1.19064 s | 2.43843 s | 4.87685 s | 9.75370 s |
| 8 | 0.12547 | — | >100% | — | — | — | — | — | — |
| 16 | 0.062734 | — | >100% | — | — | — | — | — | — |

---

## 15. Resource Scaling

The post-implementation LUT utilization increases substantially as parser instances are replicated.

| Parser Count | LUT Utilization |
|---:|---:|
| 1 | 16% |
| 2 | 33% |
| 4 | 69% |
| 8 | >100% |
| 16 | >100% |

The increase is approximately consistent with the replication of the parser hardware.

The four-parser implementation already consumes 69% of the available LUT resources.

The eight-parser implementation requires 31,196 LUTs, exceeding the 20,800 LUTs available on the target FPGA.

---

## 16. Why 8 and 16 Parsers Cannot Be Implemented

The selected FPGA contains:

20,800 LUTs

The complete eight-parser design requires:

31,196 LUTs

Therefore:

\[
31,196 > 20,800
\]

The eight-parser architecture cannot fit on the selected FPGA.

The sixteen-parser architecture would require even more resources and therefore also cannot be implemented on this device.

This is a hardware resource limitation rather than a Verilator simulation limitation.

The eight- and sixteen-parser architectures remain valid RTL simulation configurations, but they cannot be assigned an FPGA operating frequency or hardware processing time on the current FPGA.

A larger FPGA with more LUT resources would be required to implement and benchmark these configurations.

---

## 17. Verilator vs FPGA Performance

The Verilator simulation throughput should not be interpreted as FPGA throughput.

Verilator runs the RTL model on the host CPU.

The FPGA benchmark instead uses:

\[
\text{Cycles/Byte}
\]

obtained from RTL simulation and:

\[
F_{max}
\]

obtained from Vivado timing analysis.

The theoretical FPGA processing time is therefore:

\[
\boxed{
T=
\frac{
\text{Data Bytes}\times\text{Cycles/Byte}
}{
F_{max}
}
}
\]

This provides an estimate of the parser processing time assuming the FPGA receives the data at a sufficient rate and that input transfer overhead is not included.

---

## 18. Current Benchmark Status

| Architecture | Verilator | Vivado Synthesis | Vivado Implementation | Fmax | FPGA Time |
|---|:---:|:---:|:---:|---:|---:|
| 1 parser | ✓ | ✓ | ✓ | 123.457 MHz | ✓ |
| 2 parsers | ✓ | ✓ | ✓ | 120.482 MHz | ✓ |
| 4 parsers | ✓ | ✓ | ✓ | 110.497 MHz | ✓ |
| 8 parsers | ✓ | ✓ | ✗ | — | — |
| 16 parsers | ✓ | ✓ | ✗ | — | — |

---

## 19. Conclusion

The POLARIS FPGA parser demonstrates clear scaling in RTL simulation as additional parser instances are added.

The measured steady-state cycles/byte decreases from approximately:

1.00374
    ↓
0.503735
    ↓
0.250935
    ↓
0.12547
    ↓
0.062734

for 1, 2, 4, 8, and 16 parser configurations respectively.

On the selected `xc7a35tcsg324-1` FPGA, the complete 1-, 2-, and 4-parser architectures can be physically implemented.

The post-implementation LUT utilization is:

1 parser  → 16%
2 parsers → 33%
4 parsers → 69%

The 8-parser architecture requires 31,196 LUTs, exceeding the 20,800 LUTs available on the target device. The 16-parser architecture similarly exceeds the available FPGA resources.

Therefore, the current hardware benchmark is limited to 1, 2, and 4 parser configurations.

The 8- and 16-parser configurations remain available as RTL/Verilator benchmarks and can be implemented in a future experiment using a larger FPGA with sufficient LUT resources.
