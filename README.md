# HPC Log Analytics: CPU Parallelism and FPGA Parsing

Parsing and analysing large log files with several implementations of the same workload,
run on two datasets:

- **NASA** HTTP server access logs (text, one request per line)
- **Polaris** HPC job logs (CSV)

## Implementations

| Layer | NASA | Polaris |
|---|---|---|
| Serial C | `nasa/hpc/serial` | `polaris/hpc/src/serial.c` |
| OpenMP | `nasa/hpc/openmp` | `polaris/hpc/src/openmp.c` |
| MPI | `nasa/hpc/mpi` | `polaris/hpc/src/mpi.c` |
| Hybrid MPI + OpenMP | `nasa/hpc/hybrid` | `polaris/hpc/src/hybrid.c` |
| FPGA parser (Verilog FSM, 1 to 16 replicated parsers) | `nasa/fpga` | `polaris/fpga` |

Benchmarks and write-ups are in each folder's `benchmarks/`, `results/` and `docs/`.

## Layout

```
nasa/
  hpc/    C, OpenMP, MPI and hybrid parsers, Makefile, dataset generator, benchmarks
  fpga/   Verilog parsers, Verilator drivers, Vivado reports and constraints, results
polaris/
  hpc/    C, OpenMP, MPI and hybrid analytics, Makefile, benchmarks
  fpga/   Verilog parsers, Verilator drivers, testbenches, sample data, benchmark docs
```

## Build and run

Datasets are not included. Use your own copy of each. Run these from the repo root.

```bash
# NASA, CPU
(cd nasa/hpc && make) && ./nasa/hpc/serial_parser <access.log>

# Polaris, CPU
(cd polaris/hpc && make) && ./polaris/hpc/serial <polaris.csv>

# NASA, FPGA simulation (needs Verilator)
(cd nasa/fpga && make dual) && ./nasa/fpga/obj_dual/Vdual_nasa_log_parser <access.log>

# Polaris, FPGA simulation (needs Verilator)
(cd polaris/fpga && make quad) && ./polaris/fpga/obj_quad/quad_polaris_parser polaris/fpga/datasets/polaris_real_10.csv
```

Requirements: gcc with OpenMP, an MPI implementation (OpenMPI), Verilator.
The Vivado synthesis and implementation numbers are recorded in the reports and docs.
