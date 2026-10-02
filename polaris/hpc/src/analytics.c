#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/analytics.h"

#define MAX_USERS 100000
#define MAX_FIELDS 100

#define USER_HASH_SIZE 262144
#define FIELD_HASH_SIZE 128

typedef struct UserNode
{
    long username;
    int count;
    struct UserNode *next;
} UserNode;

typedef struct FieldNode
{
    char field[128];
    int count;
    struct FieldNode *next;
} FieldNode;

void dataset_summary(Job *jobs, int job_count)
{
    double total_runtime = 0;
    double total_queue_wait = 0;
    double total_nodes = 0;
    double total_cores = 0;
    double total_gpus = 0;

    int successful_jobs = 0;
    int failed_jobs = 0;

    for (int i = 0; i < job_count; i++)
    {
        total_runtime += jobs[i].runtime;
        total_queue_wait += jobs[i].queue_wait;
        total_nodes += jobs[i].nodes_used;
        total_cores += jobs[i].cores_used;

        if (jobs[i].gpus_requested > 0)
            total_gpus += jobs[i].gpus_requested;

        if (jobs[i].exit_status == 0)
            successful_jobs++;
        else
            failed_jobs++;
    }

    printf("\n========== DATASET SUMMARY ==========\n");

    printf("Total Jobs           : %d\n", job_count);
    printf("Successful Jobs      : %d\n", successful_jobs);
    printf("Failed Jobs          : %d\n", failed_jobs);

    printf("Average Runtime      : %.2f sec\n",
           total_runtime / job_count);

    printf("Average Queue Wait   : %.2f sec\n",
           total_queue_wait / job_count);

    printf("Average Nodes Used   : %.2f\n",
           total_nodes / job_count);

    printf("Average Cores Used   : %.2f\n",
           total_cores / job_count);

    printf("Average GPUs Used    : %.2f\n",
           total_gpus / job_count);
}


typedef struct
{
    long username;
    int count;
} UserCount;

typedef struct
{
    char field[128];
    int count;
} FieldCount;

typedef struct
{
    int index;
    double value;
} JobRanking;


void top_users(Job *jobs, int job_count)
{
    UserNode *table[USER_HASH_SIZE] = {0};

    for (int i = 0; i < job_count; i++)
    {
        unsigned long hash =
            (unsigned long)jobs[i].username % USER_HASH_SIZE;

        UserNode *node = table[hash];

        while (node != NULL)
        {
            if (node->username == jobs[i].username)
            {
                node->count++;
                break;
            }

            node = node->next;
        }

        if (node == NULL)
        {
            node = malloc(sizeof(UserNode));

            node->username = jobs[i].username;
            node->count = 1;

            node->next = table[hash];
            table[hash] = node;
        }
    }

    UserNode *top[10] = {0};

    for (int i = 0; i < USER_HASH_SIZE; i++)
    {
        UserNode *node = table[i];

        while (node != NULL)
        {
            for (int j = 0; j < 10; j++)
            {
                if (top[j] == NULL || node->count > top[j]->count)
                {
                    for (int k = 9; k > j; k--)
                        top[k] = top[k - 1];

                    top[j] = node;
                    break;
                }
            }

            node = node->next;
        }
    }

    printf("\n========== TOP 10 USERS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        if (top[i] != NULL)
        {
            printf("%2d. User: %ld | Jobs: %d\n",
                   i + 1,
                   top[i]->username,
                   top[i]->count);
        }
    }

    for (int i = 0; i < USER_HASH_SIZE; i++)
    {
        UserNode *node = table[i];

        while (node != NULL)
        {
            UserNode *next = node->next;
            free(node);
            node = next;
        }
    }
}


void top_science_fields(Job *jobs, int job_count)
{
    FieldNode *table[FIELD_HASH_SIZE] = {0};

    for (int i = 0; i < job_count; i++)
    {
        if (strcmp(jobs[i].science_field, "-1") == 0 ||
            strcmp(jobs[i].science_field, "0") == 0 ||
            strlen(jobs[i].science_field) == 0)
        {
            continue;
        }

        unsigned long hash = 0;

        for (int j = 0; jobs[i].science_field[j] != '\0'; j++)
        {
            hash = hash * 31 + jobs[i].science_field[j];
        }

        hash %= FIELD_HASH_SIZE;

        FieldNode *node = table[hash];

        while (node != NULL)
        {
            if (strcmp(node->field, jobs[i].science_field) == 0)
            {
                node->count++;
                break;
            }

            node = node->next;
        }

        if (node == NULL)
        {
            node = malloc(sizeof(FieldNode));

            strcpy(node->field, jobs[i].science_field);
            node->count = 1;

            node->next = table[hash];
            table[hash] = node;
        }
    }

    FieldNode *top[10] = {0};

    for (int i = 0; i < FIELD_HASH_SIZE; i++)
    {
        FieldNode *node = table[i];

        while (node != NULL)
        {
            for (int j = 0; j < 10; j++)
            {
                if (top[j] == NULL || node->count > top[j]->count)
                {
                    for (int k = 9; k > j; k--)
                        top[k] = top[k - 1];

                    top[j] = node;
                    break;
                }
            }

            node = node->next;
        }
    }

    printf("\n========== TOP 10 SCIENCE FIELDS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        if (top[i] != NULL)
        {
            printf("%2d. %-40s | Jobs: %d\n",
                   i + 1,
                   top[i]->field,
                   top[i]->count);
        }
    }

    for (int i = 0; i < FIELD_HASH_SIZE; i++)
    {
        FieldNode *node = table[i];

        while (node != NULL)
        {
            FieldNode *next = node->next;
            free(node);
            node = next;
        }
    }
}

void longest_running_jobs(Job *jobs, int job_count)
{
    JobRanking top[10] = {0};

    for (int i = 0; i < job_count; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (jobs[i].runtime > top[j].value)
            {
                for (int k = 9; k > j; k--)
                    top[k] = top[k - 1];

                top[j].index = i;
                top[j].value = jobs[i].runtime;

                break;
            }
        }
    }

    printf("\n========== TOP 10 LONGEST RUNNING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[top[i].index];

        printf("%2d. %-20s | Runtime: %.2f sec | Nodes: %.0f | Cores: %.0f\n",
               i + 1,
               job->job_name,
               job->runtime,
               job->nodes_used,
               job->cores_used);
    }
}


void longest_queue_waits(Job *jobs, int job_count)
{
    JobRanking top[10] = {0};

    for (int i = 0; i < job_count; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (jobs[i].queue_wait > top[j].value)
            {
                for (int k = 9; k > j; k--)
                    top[k] = top[k - 1];

                top[j].index = i;
                top[j].value = jobs[i].queue_wait;

                break;
            }
        }
    }

    printf("\n========== TOP 10 LONGEST QUEUE WAITS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[top[i].index];

        printf("%2d. %-20s | Queue Wait: %.2f sec | Nodes: %.0f | Cores: %.0f\n",
               i + 1,
               job->job_name,
               job->queue_wait,
               job->nodes_used,
               job->cores_used);
    }
}

void largest_resource_jobs(Job *jobs, int job_count)
{
    JobRanking top_nodes[10] = {0};
    JobRanking top_cores[10] = {0};
    JobRanking top_gpus[10] = {0};

    for (int i = 0; i < job_count; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (jobs[i].nodes_used > top_nodes[j].value)
            {
                for (int k = 9; k > j; k--)
                    top_nodes[k] = top_nodes[k - 1];

                top_nodes[j].index = i;
                top_nodes[j].value = jobs[i].nodes_used;
                break;
            }
        }

        for (int j = 0; j < 10; j++)
        {
            if (jobs[i].cores_used > top_cores[j].value)
            {
                for (int k = 9; k > j; k--)
                    top_cores[k] = top_cores[k - 1];

                top_cores[j].index = i;
                top_cores[j].value = jobs[i].cores_used;
                break;
            }
        }

        for (int j = 0; j < 10; j++)
        {
            if (jobs[i].gpus_requested > top_gpus[j].value)
            {
                for (int k = 9; k > j; k--)
                    top_gpus[k] = top_gpus[k - 1];

                top_gpus[j].index = i;
                top_gpus[j].value = jobs[i].gpus_requested;
                break;
            }
        }
    }

    printf("\n========== TOP 10 NODE-USING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[top_nodes[i].index];

        printf("%2d. %-20s | Nodes: %.0f | Cores: %.0f | GPUs: %.0f\n",
               i + 1,
               job->job_name,
               job->nodes_used,
               job->cores_used,
               job->gpus_requested);
    }

    printf("\n========== TOP 10 CORE-USING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[top_cores[i].index];

        printf("%2d. %-20s | Cores: %.0f | Nodes: %.0f | GPUs: %.0f\n",
               i + 1,
               job->job_name,
               job->cores_used,
               job->nodes_used,
               job->gpus_requested);
    }

    printf("\n========== TOP 10 GPU-REQUESTING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[top_gpus[i].index];

        printf("%2d. %-20s | GPUs: %.0f | Nodes: %.0f | Cores: %.0f\n",
               i + 1,
               job->job_name,
               job->gpus_requested,
               job->nodes_used,
               job->cores_used);
    }
}
