#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/csv_reader.h"
#include "../include/analytics.h"

static double get_time()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <dataset.csv>\n", argv[0]);
        return 1;
    }

    int job_count = 0;

    double start, end;

    /* CSV reading + parsing */

    start = get_time();

    Job *jobs = read_csv(argv[1], &job_count);

    end = get_time();

    if (jobs == NULL)
    {
        printf("Failed to load dataset.\n");
        return 1;
    }

    printf("\n========== PERFORMANCE BREAKDOWN ==========\n");

    printf("CSV Read + Parsing    : %.6f sec\n",
           end - start);

    /* Dataset summary */

    start = get_time();

    dataset_summary(jobs, job_count);

    end = get_time();

    printf("Dataset Summary       : %.6f sec\n",
           end - start);

    /* Users */

    start = get_time();

    top_users(jobs, job_count);

    end = get_time();

    printf("Top Users             : %.6f sec\n",
           end - start);

    /* Science fields */

    start = get_time();

    top_science_fields(jobs, job_count);

    end = get_time();

    printf("Science Fields        : %.6f sec\n",
           end - start);

    /* Runtime */

    start = get_time();

    longest_running_jobs(jobs, job_count);

    end = get_time();

    printf("Longest Runtime       : %.6f sec\n",
           end - start);

    /* Queue */

    start = get_time();

    longest_queue_waits(jobs, job_count);

    end = get_time();

    printf("Queue Wait Analysis   : %.6f sec\n",
           end - start);

    /* Resources */

    start = get_time();

    largest_resource_jobs(jobs, job_count);

    end = get_time();

    printf("Resource Analysis     : %.6f sec\n",
           end - start);

    free(jobs);

    return 0;
}
