.PHONY: all serial openmp mpi hybrid clean

CC=gcc
MPICC=mpicc

CFLAGS=-O3 -Wall -Iinclude
OMPFLAGS=-fopenmp

COMMON_SRC=src/parser.c \
           src/timer.c \
           src/analytics.c \
           src/ip_hash.c \
           src/ip_counter.c

SERIAL_SRC=$(COMMON_SRC) serial/main.c
OPENMP_SRC=$(COMMON_SRC) openmp/main.c
MPI_SRC=$(COMMON_SRC) mpi/main.c
HYBRID_SRC=$(COMMON_SRC) hybrid/main.c

all: serial openmp mpi hybrid

serial:
	$(CC) $(CFLAGS) $(SERIAL_SRC) -o serial_parser

openmp:
	$(CC) $(CFLAGS) $(OMPFLAGS) $(OPENMP_SRC) -o openmp_parser

mpi:
	$(MPICC) $(CFLAGS) $(MPI_SRC) -o mpi_parser

hybrid:
	$(MPICC) $(CFLAGS) $(OMPFLAGS) $(HYBRID_SRC) -o hybrid_parser

clean:
	rm -f serial_parser openmp_parser mpi_parser hybrid_parser
