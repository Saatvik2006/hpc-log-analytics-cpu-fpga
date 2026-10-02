#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <mpi.h>
#include <omp.h>

#include "../include/job.h"

#define MAX_LINE_LENGTH 4096

/* ============================================================
   CSV PARSING
   ============================================================ */

static int parse_csv_line(const char *line, char fields[][256])
{
    int field = 0;
    int pos = 0;
    int in_quotes = 0;

    while (*line && field < 65)
    {
        if (*line == '"')
        {
            in_quotes = !in_quotes;
        }
        else if (*line == ',' && !in_quotes)
        {
            fields[field][pos] = '\0';
            field++;
            pos = 0;
        }
        else if (*line != '\n' && *line != '\r')
        {
            if (pos < 255)
                fields[field][pos++] = *line;
        }

        line++;
    }

    fields[field][pos] = '\0';

    return field + 1;
}

static void parse_line(const char *line, Job *job)
{
    char fields[66][256];

    parse_csv_line(line, fields);

    strcpy(job->job_name, fields[0]);

    job->username = atol(fields[9]);

    job->runtime = atof(fields[14]);

    job->nodes_used = atof(fields[15]);
    job->nodes_requested = atof(fields[16]);

    job->cores_used = atof(fields[17]);
    job->cores_requested = atof(fields[18]);

    job->exit_status = atoi(fields[20]);

    job->queue_wait = atof(fields[23]);

    job->gpus_requested = atof(fields[58]);

    if (job->gpus_requested < 0)
        job->gpus_requested = 0;

    strcpy(job->science_field, fields[64]);
}


/* ============================================================
   HYBRID MPI + OPENMP CSV READER
   ============================================================ */

static Job *read_local_chunk_hybrid(
    const char *filename,
    int rank,
    int world_size,
    int num_threads,
    int *job_count,
    double *parse_time)
{
    struct stat file_info;

    if (stat(filename, &file_info) != 0)
    {
        perror("stat");
        return NULL;
    }

    long long file_size = file_info.st_size;

    long long rank_start =
        (file_size * rank) / world_size;

    long long rank_end =
        (file_size * (rank + 1)) / world_size;

    /*
     * First pass:
     * determine how many complete rows belong to this MPI rank.
     *
     * Each OpenMP thread scans its own byte range.
     */

    int *thread_counts =
        calloc(num_threads, sizeof(int));

    if (thread_counts == NULL)
        return NULL;

    double start_time = MPI_Wtime();

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        long long thread_start =
            rank_start +
            ((rank_end - rank_start) * tid) / num_threads;

        long long thread_end =
            rank_start +
            ((rank_end - rank_start) * (tid + 1)) / num_threads;

        FILE *fp = fopen(filename, "r");

        if (fp == NULL)
        {
            fprintf(stderr,
                    "Rank %d Thread %d: fopen failed\n",
                    rank, tid);
        }
        else
        {
            char line[MAX_LINE_LENGTH];

            fseeko(fp, thread_start, SEEK_SET);

            /*
             * Every thread must align itself to a complete row.
             *
             * Thread 0 of rank 0 skips the CSV header.
             * Every other thread discards its partial first row.
             */
            if (thread_start == 0)
            {
                if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                {
                    fclose(fp);
                }
                else
                {
                    int count = 0;

                    while (1)
                    {
                        long long pos = ftello(fp);

                        if (pos >= thread_end)
                            break;

                        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                            break;

                        count++;
                    }

                    thread_counts[tid] = count;
                    fclose(fp);
                }
            }
            else
            {
                /*
                 * Move to the beginning of the next complete row.
                 */
                if (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
                {
                    int count = 0;

                    while (1)
                    {
                        long long pos = ftello(fp);

                        if (pos >= thread_end)
                            break;

                        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                            break;

                        count++;
                    }

                    thread_counts[tid] = count;
                }

                fclose(fp);
            }
        }
    }

    int total_count = 0;

    for (int i = 0; i < num_threads; i++)
        total_count += thread_counts[i];

    /*
     * Allocate the exact amount of storage required by this rank.
     */

    Job *jobs = malloc(
        (size_t)total_count * sizeof(Job)
    );

    if (jobs == NULL && total_count > 0)
    {
        free(thread_counts);
        return NULL;
    }

    /*
     * Second pass:
     * each OpenMP thread parses its own byte range
     * directly into its assigned section of the Job array.
     */

    int *thread_offsets =
        malloc(num_threads * sizeof(int));

    if (thread_offsets == NULL)
    {
        free(jobs);
        free(thread_counts);
        return NULL;
    }

    thread_offsets[0] = 0;

    for (int i = 1; i < num_threads; i++)
    {
        thread_offsets[i] =
            thread_offsets[i - 1] +
            thread_counts[i - 1];
    }

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        long long thread_start =
            rank_start +
            ((rank_end - rank_start) * tid) / num_threads;

        long long thread_end =
            rank_start +
            ((rank_end - rank_start) * (tid + 1)) / num_threads;

        FILE *fp = fopen(filename, "r");

        if (fp != NULL)
        {
            char line[MAX_LINE_LENGTH];

            fseeko(fp, thread_start, SEEK_SET);

            /*
             * Align to a complete row.
             */
            if (thread_start == 0)
            {
                if (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
                {
                    int index = thread_offsets[tid];

                    while (1)
                    {
                        long long pos = ftello(fp);

                        if (pos >= thread_end)
                            break;

                        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                            break;

                        parse_line(line, &jobs[index]);
                        index++;
                    }
                }
            }
            else
            {
                if (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
                {
                    int index = thread_offsets[tid];

                    while (1)
                    {
                        long long pos = ftello(fp);

                        if (pos >= thread_end)
                            break;

                        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                            break;

                        parse_line(line, &jobs[index]);
                        index++;
                    }
                }
            }

            fclose(fp);
        }
    }

    *job_count = total_count;

    *parse_time =
        MPI_Wtime() - start_time;

    free(thread_offsets);
    free(thread_counts);

    return jobs;
}


/* ============================================================
   LOCAL SUMMARY
   ============================================================ */

static void local_summary(
    Job *jobs,
    int count,
    double *runtime,
    double *queue_wait,
    double *nodes,
    double *cores,
    double *gpus,
    int *successful,
    int *failed)
{
    double r = 0.0;
    double q = 0.0;
    double n = 0.0;
    double c = 0.0;
    double g = 0.0;

    int s = 0;
    int f = 0;

    #pragma omp parallel for \
        reduction(+:r,q,n,c,g,s,f)
    for (int i = 0; i < count; i++)
    {
        r += jobs[i].runtime;
        q += jobs[i].queue_wait;
        n += jobs[i].nodes_used;
        c += jobs[i].cores_used;

        if (jobs[i].gpus_requested > 0)
            g += jobs[i].gpus_requested;

        if (jobs[i].exit_status == 0)
            s++;
        else
            f++;
    }

    *runtime = r;
    *queue_wait = q;
    *nodes = n;
    *cores = c;
    *gpus = g;

    *successful = s;
    *failed = f;
}

/* ============================================================
   MAIN
   ============================================================ */

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf(
            "Usage: %s <dataset.csv> <threads_per_rank>\n",
            argv[0]
        );

        return 1;
    }

    int provided;

    MPI_Init_thread(
        &argc,
        &argv,
        MPI_THREAD_FUNNELED,
        &provided
    );

    int rank;
    int world_size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const char *filename = argv[1];

    int num_threads = atoi(argv[2]);

    if (num_threads < 1)
        num_threads = 1;

    omp_set_num_threads(num_threads);

    MPI_Barrier(MPI_COMM_WORLD);

    /*
     * --------------------------------------------------------
     * HYBRID CSV PARSING
     * --------------------------------------------------------
     */

    int local_count = 0;

    double parse_time = 0.0;

    Job *jobs = read_local_chunk_hybrid(
        filename,
        rank,
        world_size,
        num_threads,
        &local_count,
        &parse_time
    );

    if (jobs == NULL)
    {
        fprintf(
            stderr,
            "Rank %d failed to read its chunk.\n",
            rank
        );

        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    /*
     * --------------------------------------------------------
     * DATASET SUMMARY
     * --------------------------------------------------------
     */

    double local_runtime;
    double local_queue_wait;
    double local_nodes;
    double local_cores;
    double local_gpus;

    int local_successful;
    int local_failed;

    local_summary(
        jobs,
        local_count,
        &local_runtime,
        &local_queue_wait,
        &local_nodes,
        &local_cores,
        &local_gpus,
        &local_successful,
        &local_failed
    );

    int total_jobs = 0;
    int total_successful = 0;
    int total_failed = 0;

    double total_runtime = 0.0;
    double total_queue_wait = 0.0;
    double total_nodes = 0.0;
    double total_cores = 0.0;
    double total_gpus = 0.0;

    MPI_Reduce(
        &local_count,
        &total_jobs,
        1,
        MPI_INT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_successful,
        &total_successful,
        1,
        MPI_INT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_failed,
        &total_failed,
        1,
        MPI_INT,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_runtime,
        &total_runtime,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_queue_wait,
        &total_queue_wait,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_nodes,
        &total_nodes,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_cores,
        &total_cores,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    MPI_Reduce(
        &local_gpus,
        &total_gpus,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        0,
        MPI_COMM_WORLD
    );

    /*
     * --------------------------------------------------------
     * OUTPUT
     * --------------------------------------------------------
     */

    double max_parse_time = 0.0;

    MPI_Reduce(
        &parse_time,
        &max_parse_time,
        1,
        MPI_DOUBLE,
        MPI_MAX,
        0,
        MPI_COMM_WORLD
    );

    if (rank == 0)
    {
        printf(
            "\n========== HYBRID DATASET SUMMARY ==========\n"
        );

        printf(
            "MPI Processes        : %d\n",
            world_size
        );

        printf(
            "OpenMP Threads/Rank  : %d\n",
            num_threads
        );

        printf(
            "Total Jobs           : %d\n",
            total_jobs
        );

        printf(
            "Successful Jobs      : %d\n",
            total_successful
        );

        printf(
            "Failed Jobs          : %d\n",
            total_failed
        );

        printf(
            "Average Runtime      : %.2f sec\n",
            total_runtime / total_jobs
        );

        printf(
            "Average Queue Wait   : %.2f sec\n",
            total_queue_wait / total_jobs
        );

        printf(
            "Average Nodes Used   : %.2f\n",
            total_nodes / total_jobs
        );

        printf(
            "Average Cores Used   : %.2f\n",
            total_cores / total_jobs
        );

        printf(
            "Average GPUs Used    : %.2f\n",
            total_gpus / total_jobs
        );

        printf(
            "\n========== HYBRID PERFORMANCE ==========\n"
        );

        printf(
            "Max CSV parsing time : %.6f sec\n",
            max_parse_time
        );
    }

    free(jobs);

    MPI_Finalize();

    return 0;
}
