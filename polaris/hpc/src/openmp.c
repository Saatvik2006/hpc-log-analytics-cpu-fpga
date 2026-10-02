#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "../include/csv_reader.h"
#include "../include/openmp_analytics.h"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <dataset.csv> <threads>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0)
    {
        printf("Invalid thread count.\n");
        return 1;
    }

    int job_count = 0;

    double start = omp_get_wtime();

    Job *jobs = read_csv_parallel(
        filename,
        &job_count,
        num_threads
    );

    double read_time = omp_get_wtime() - start;

    if (jobs == NULL)
    {
        printf("Failed to load dataset.\n");
        return 1;
    }

    start = omp_get_wtime();

    /*
     * Existing analytics calls
     */
    openmp_analysis(
        jobs,
        job_count,
        num_threads
    );

    openmp_top_users(
        jobs,
        job_count,
        num_threads
    );

    openmp_top_science_fields(
        jobs,
        job_count,
        num_threads
    );

    openmp_resource_analysis(
        jobs,
        job_count,
        num_threads
    );

    double analytics_time = omp_get_wtime() - start;

    printf("\n========== PERFORMANCE BREAKDOWN ==========\n");
    printf("CSV Read + Parsing + Merge : %.6f sec\n", read_time);
    printf("Analytics                  : %.6f sec\n", analytics_time);
    printf("Total                      : %.6f sec\n",
           read_time + analytics_time);

    free(jobs);

    return 0;
}
