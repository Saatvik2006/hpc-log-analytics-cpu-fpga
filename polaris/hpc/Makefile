CC      = gcc
MPICC   = mpicc
CFLAGS  = -O2
OMP     = -fopenmp

.PHONY: all clean
all: serial openmp mpi hybrid

# csv_reader.c calls OpenMP functions, so even the serial build links with -fopenmp
serial: src/serial.c src/csv_reader.c src/analytics.c
	$(CC) $(CFLAGS) $(OMP) src/serial.c src/csv_reader.c src/analytics.c -o serial

openmp: src/openmp.c src/csv_reader.c src/openmp_analytics.c
	$(CC) $(CFLAGS) $(OMP) src/openmp.c src/csv_reader.c src/openmp_analytics.c -o openmp

# mpi.c and hybrid.c are self-contained
mpi: src/mpi.c
	$(MPICC) $(CFLAGS) src/mpi.c -o mpi

hybrid: src/hybrid.c
	$(MPICC) $(CFLAGS) $(OMP) src/hybrid.c -o hybrid

clean:
	rm -f serial openmp mpi hybrid
