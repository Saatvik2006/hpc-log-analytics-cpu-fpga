#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "../include/parser.h"
#include "../include/analytics.h"
#include "../include/ip_hash.h"
#include "../include/ip_counter.h"

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2)
    {
        if (rank == 0)
            printf("Usage: %s <logfile>\n", argv[0]);

        MPI_Finalize();
        return 1;
    }

    long filesize = 0;

    if (rank == 0)
    {
        FILE *fp = fopen(argv[1], "rb");

        if (fp == NULL)
        {
            perror("Error opening file");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        fseek(fp, 0, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);
    }

    MPI_Bcast(&filesize, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    long chunk = filesize / size;

    long start_pos = rank * chunk;
    long end_pos = (rank == size - 1) ? filesize : (rank + 1) * chunk;

    FILE *file = fopen(argv[1], "rb");

    if (file == NULL)
    {
        perror("Error opening file");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    fseek(file, start_pos, SEEK_SET);

    if (start_pos != 0)
    {
        fseek(file, start_pos - 1, SEEK_SET);

        if (fgetc(file) != '\n')
        {
            char dummy[2048];
            fgets(dummy, sizeof(dummy), file);
        }
    }

    Analytics local;
    analytics_init(&local);

    IPHash local_table;
    ip_hash_init(&local_table);

    char line[2048];
    LogEntry entry;

    MPI_Barrier(MPI_COMM_WORLD);

    double start = MPI_Wtime();

    while (ftell(file) < end_pos && fgets(line, sizeof(line), file))
    {
        if (parse_log_line(line, &entry))
        {
            analytics_update(&local, &entry);
            ip_counter_update(&local_table, &entry);
        }
    }

    fclose(file);

    Analytics global;

    MPI_Reduce(&local.parsed_lines,
               &global.parsed_lines,
               9,
               MPI_LONG,
               MPI_SUM,
               0,
               MPI_COMM_WORLD);

    int local_count = ip_hash_count_entries(&local_table);

    if (rank == 0)
    {
        IPHash global_table;
        ip_hash_init(&global_table);

        ip_hash_merge(&global_table, &local_table);

        for (int src = 1; src < size; src++)
        {
            int count;

            MPI_Recv(&count,
                     1,
                     MPI_INT,
                     src,
                     0,
                     MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            if (count == 0)
                continue;

            IPResult *buffer = malloc(count * sizeof(IPResult));

            MPI_Recv(buffer,
                     count * sizeof(IPResult),
                     MPI_BYTE,
                     src,
                     1,
                     MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);

            for (int i = 0; i < count; i++)
            {
                ip_hash_insert_count(&global_table,
                                     buffer[i].ip,
                                     buffer[i].count);
            }

            free(buffer);
        }

        double end = MPI_Wtime();

        analytics_print(&global, end - start);

        printf("\n");
        printf("=========================================\n");
        printf("Top 10 IP Addresses\n");
        printf("=========================================\n");

        ip_hash_print_top(&global_table, 10);

        ip_hash_destroy(&global_table);
    }
    else
    {
        MPI_Send(&local_count,
                 1,
                 MPI_INT,
                 0,
                 0,
                 MPI_COMM_WORLD);

        if (local_count > 0)
        {
            IPResult *buffer = malloc(local_count * sizeof(IPResult));

            ip_hash_export(&local_table, buffer);

            MPI_Send(buffer,
                     local_count * sizeof(IPResult),
                     MPI_BYTE,
                     0,
                     1,
                     MPI_COMM_WORLD);

            free(buffer);
        }
    }

    ip_hash_destroy(&local_table);

    MPI_Finalize();

    return 0;
}
