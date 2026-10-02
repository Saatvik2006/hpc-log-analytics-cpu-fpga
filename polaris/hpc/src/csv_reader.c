#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <omp.h>

#include "../include/csv_reader.h"

#define MAX_LINE_LENGTH 4096
#define INITIAL_CAPACITY 1000

static void remove_quotes(char *str)
{
    int len = strlen(str);

    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

static int parse_csv_line(const char *line, char fields[][256])
{
    int field = 0;
    int pos = 0;
    int in_quotes = 0;

    while (*line)
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

Job *read_csv(const char *filename, int *job_count)
{
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return NULL;
    }

    int capacity = INITIAL_CAPACITY;

    Job *jobs = malloc(capacity * sizeof(Job));

    char line[MAX_LINE_LENGTH];

    *job_count = 0;

    // Skip header
    if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
    {
        fclose(fp);
        free(jobs);
        return NULL;
    }

    while (fgets(line, MAX_LINE_LENGTH, fp))
    {
        if (*job_count >= capacity)
        {
            capacity *= 2;
            jobs = realloc(jobs, capacity * sizeof(Job));
        }

        parse_line(line, &jobs[*job_count]);

        (*job_count)++;
    }

    fclose(fp);

    return jobs;
}

Job *read_csv_parallel(const char *filename,
                       int *job_count,
                       int num_threads)
{
    struct stat file_info;

    if (stat(filename, &file_info) != 0)
    {
        perror("stat");
        return NULL;
    }

    long long file_size = file_info.st_size;

    /*
     * Each thread stores how many complete rows
     * exist in its assigned chunk.
     */
    int *thread_counts = calloc(
        num_threads,
        sizeof(int)
    );

    if (thread_counts == NULL)
    {
        return NULL;
    }

    /*
     * ------------------------------------------------
     * PASS 1: Count rows in each chunk
     * ------------------------------------------------
     */

    double pass1_start = omp_get_wtime();

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        FILE *fp = fopen(filename, "r");

        if (fp == NULL)
        {
            printf("Thread %d: failed to open file\n", tid);
        }
        else
        {
            long long start =
                (file_size * tid) / num_threads;

            long long end =
                (file_size * (tid + 1)) / num_threads;

            fseeko(fp, start, SEEK_SET);

            /*
             * Thread 0 handles the CSV header.
             * Other threads may start in the middle
             * of a CSV row, so discard that partial row.
             */
            char line[MAX_LINE_LENGTH];

            if (start == 0)
            {
                if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                {
                    fclose(fp);
                    goto count_done;
                }
            }
            else
            {
                if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                {
                    fclose(fp);
                    goto count_done;
                }
            }

            int count = 0;

            while (1)
            {
                long long line_start = ftello(fp);

                if (tid != num_threads - 1 &&
                    line_start >= end)
                {
                    break;
                }

                if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                {
                    break;
                }

                count++;
            }

            thread_counts[tid] = count;

            fclose(fp);
        }

count_done:
        ;
    }

    double pass1_time =
        omp_get_wtime() - pass1_start;

    /*
     * Calculate total jobs.
     */
    int total_jobs = 0;

    for (int i = 0; i < num_threads; i++)
    {
        total_jobs += thread_counts[i];
    }

    /*
     * Calculate where each thread should write
     * inside the final Job array.
     */
    int *thread_offsets = calloc(
        num_threads,
        sizeof(int)
    );

    if (thread_offsets == NULL)
    {
        free(thread_counts);
        return NULL;
    }

    for (int i = 1; i < num_threads; i++)
    {
        thread_offsets[i] =
            thread_offsets[i - 1] +
            thread_counts[i - 1];
    }

    /*
     * Allocate the final Job array exactly once.
     */
    Job *jobs = malloc(
        total_jobs * sizeof(Job)
    );

    if (jobs == NULL)
    {
        free(thread_counts);
        free(thread_offsets);
        return NULL;
    }

    /*
     * ------------------------------------------------
     * PASS 2: Parse rows directly into final array
     * ------------------------------------------------
     */

    double pass2_start = omp_get_wtime();

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        FILE *fp = fopen(filename, "r");

        if (fp == NULL)
        {
            printf("Thread %d: failed to open file\n", tid);
        }
        else
        {
            long long start =
                (file_size * tid) / num_threads;

            long long end =
                (file_size * (tid + 1)) / num_threads;

            fseeko(fp, start, SEEK_SET);

            char line[MAX_LINE_LENGTH];

            /*
             * Skip header / partial row.
             */
            if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
            {
                fclose(fp);
                goto parse_done;
            }

            /*
             * Each thread starts writing exactly
             * at its own predetermined position.
             */
            int index = thread_offsets[tid];

            while (1)
            {
                long long line_start = ftello(fp);

                if (tid != num_threads - 1 &&
                    line_start >= end)
                {
                    break;
                }

                if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
                {
                    break;
                }

                parse_line(
                    line,
                    &jobs[index]
                );

                index++;
            }

            fclose(fp);
        }

parse_done:
        ;
    }

    double pass2_time =
        omp_get_wtime() - pass2_start;

    printf("Parallel CSV pass 1      : %.6f sec\n",
           pass1_time);

    printf("Parallel CSV pass 2      : %.6f sec\n",
           pass2_time);

    printf("Parallel CSV total       : %.6f sec\n",
           pass1_time + pass2_time);

    free(thread_counts);
    free(thread_offsets);

    *job_count = total_jobs;

    return jobs;
}
