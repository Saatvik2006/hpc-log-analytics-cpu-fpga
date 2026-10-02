#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#include "../include/parser.h"
#include "../include/analytics.h"

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2)
    {
        if(rank == 0)
            printf("Usage: %s <logfile>\n", argv[0]);

        MPI_Finalize();
        return 1;
    }

    long filesize = 0;

    if(rank == 0)
    {
        FILE *fp = fopen(argv[1], "rb");

        if(fp == NULL)
        {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD,1);
        }

        fseek(fp,0,SEEK_END);
        filesize = ftell(fp);
        fclose(fp);
    }

    MPI_Bcast(&filesize,1,MPI_LONG,0,MPI_COMM_WORLD);

    long chunk = filesize / size;

    long start_pos = rank * chunk;
    long end_pos   = (rank == size-1) ? filesize : (rank+1)*chunk;

    Analytics rank_stats;
    analytics_init(&rank_stats);

    MPI_Barrier(MPI_COMM_WORLD);

    double start = MPI_Wtime();

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int threads = omp_get_num_threads();

        FILE *file = fopen(argv[1], "rb");

        if(file == NULL)
        {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD,1);
        }

        long rank_chunk = end_pos - start_pos;
        long thread_chunk = rank_chunk / threads;

        long thread_start = start_pos + tid * thread_chunk;
        long thread_end = (tid == threads - 1)
                            ? end_pos
                            : thread_start + thread_chunk;

        fseek(file, thread_start, SEEK_SET);

        if(thread_start != 0)
        {
            fseek(file, thread_start - 1, SEEK_SET);

            if(fgetc(file) != '\n')
            {
                char dummy[2048];
                fgets(dummy, sizeof(dummy), file);
            }
        }

        Analytics local_stats;
        analytics_init(&local_stats);

        char line[2048];
        LogEntry entry;

        while(ftell(file) < thread_end &&
              fgets(line, sizeof(line), file))
        {
            if(parse_log_line(line, &entry))
            {
                analytics_update(&local_stats, &entry);
            }
        }

        fclose(file);

        #pragma omp critical
        {
            analytics_merge(&rank_stats, &local_stats);
        }
    }

    Analytics global_stats;

    MPI_Reduce(&rank_stats.parsed_lines,
               &global_stats.parsed_lines,
               9,
               MPI_LONG,
               MPI_SUM,
               0,
               MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if(rank == 0)
    {
        analytics_print(&global_stats, end - start);
    }

    MPI_Finalize();

    return 0;
}
