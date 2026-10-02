#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "../include/openmp_analytics.h"

#define USER_HASH_SIZE 65536
#define FIELD_HASH_SIZE 128

void openmp_analysis(Job *jobs, int job_count, int num_threads)
{
    double total_runtime = 0.0;
    double total_queue_wait = 0.0;
    double total_nodes = 0.0;
    double total_cores = 0.0;
    double total_gpus = 0.0;

    int successful_jobs = 0;
    int failed_jobs = 0;

    omp_set_num_threads(num_threads);

    #pragma omp parallel for reduction(+:total_runtime, total_queue_wait, \
                                       total_nodes, total_cores, total_gpus, \
                                       successful_jobs, failed_jobs)
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

    printf("\n========== OPENMP DATASET SUMMARY ==========\n");

    printf("Threads              : %d\n", num_threads);
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

typedef struct
{
    long username;
    int count;
} UserResult;

typedef struct
{
    char field[128];
    int count;
} FieldResult;

void openmp_top_users(Job *jobs, int job_count, int num_threads)
{
    UserNode ***local_tables = malloc(
        num_threads * sizeof(UserNode **)
    );

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        UserNode **table = calloc(
            USER_HASH_SIZE,
            sizeof(UserNode *)
        );

        #pragma omp for
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

        local_tables[tid] = table;
    }

    /*
     * Merge all thread-local hash tables
     */

    UserNode **global_table = calloc(
        USER_HASH_SIZE,
        sizeof(UserNode *)
    );

    for (int t = 0; t < num_threads; t++)
    {
        UserNode **table = local_tables[t];

        for (int i = 0; i < USER_HASH_SIZE; i++)
        {
            UserNode *node = table[i];

            while (node != NULL)
            {
                unsigned long hash =
                    (unsigned long)node->username % USER_HASH_SIZE;

                UserNode *global_node = global_table[hash];

                while (global_node != NULL)
                {
                    if (global_node->username == node->username)
                    {
                        global_node->count += node->count;
                        break;
                    }

                    global_node = global_node->next;
                }

                if (global_node == NULL)
                {
                    global_node = malloc(sizeof(UserNode));

                    global_node->username = node->username;
                    global_node->count = node->count;

                    global_node->next = global_table[hash];
                    global_table[hash] = global_node;
                }

                node = node->next;
            }
        }
    }

    /*
     * Find global top 10
     */

    UserNode *top[10] = {0};

    for (int i = 0; i < USER_HASH_SIZE; i++)
    {
        UserNode *node = global_table[i];

        while (node != NULL)
        {
            for (int j = 0; j < 10; j++)
            {
                if (top[j] == NULL ||
                    node->count > top[j]->count)
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

    printf("\n========== OPENMP TOP 10 USERS ==========\n");

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

    /*
     * Free thread-local tables
     */

    for (int t = 0; t < num_threads; t++)
    {
        UserNode **table = local_tables[t];

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

        free(table);
    }

    /*
     * Free global table
     */

    for (int i = 0; i < USER_HASH_SIZE; i++)
    {
        UserNode *node = global_table[i];

        while (node != NULL)
        {
            UserNode *next = node->next;
            free(node);
            node = next;
        }
    }

    free(global_table);
    free(local_tables);
}

void openmp_top_science_fields(Job *jobs, int job_count, int num_threads)
{
    FieldNode ***local_tables = malloc(
        num_threads * sizeof(FieldNode **)
    );

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        FieldNode **table = calloc(
            FIELD_HASH_SIZE,
            sizeof(FieldNode *)
        );

        #pragma omp for
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

        local_tables[tid] = table;
    }

    /* Merge local tables */

    FieldNode **global_table = calloc(
        FIELD_HASH_SIZE,
        sizeof(FieldNode *)
    );

    for (int t = 0; t < num_threads; t++)
    {
        FieldNode **table = local_tables[t];

        for (int i = 0; i < FIELD_HASH_SIZE; i++)
        {
            FieldNode *node = table[i];

            while (node != NULL)
            {
                unsigned long hash = 0;

                for (int j = 0; node->field[j] != '\0'; j++)
                {
                    hash = hash * 31 + node->field[j];
                }

                hash %= FIELD_HASH_SIZE;

                FieldNode *global_node = global_table[hash];

                while (global_node != NULL)
                {
                    if (strcmp(global_node->field, node->field) == 0)
                    {
                        global_node->count += node->count;
                        break;
                    }

                    global_node = global_node->next;
                }

                if (global_node == NULL)
                {
                    global_node = malloc(sizeof(FieldNode));

                    strcpy(global_node->field, node->field);
                    global_node->count = node->count;

                    global_node->next = global_table[hash];
                    global_table[hash] = global_node;
                }

                node = node->next;
            }
        }
    }

    /* Find top 10 */

    FieldNode *top[10] = {0};

    for (int i = 0; i < FIELD_HASH_SIZE; i++)
    {
        FieldNode *node = global_table[i];

        while (node != NULL)
        {
            for (int j = 0; j < 10; j++)
            {
                if (top[j] == NULL ||
                    node->count > top[j]->count)
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

    printf("\n========== OPENMP TOP 10 SCIENCE FIELDS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        if (top[i] != NULL)
        {
            printf("%2d. %-50s | Jobs: %d\n",
                   i + 1,
                   top[i]->field,
                   top[i]->count);
        }
    }

    /* Free local tables */

    for (int t = 0; t < num_threads; t++)
    {
        FieldNode **table = local_tables[t];

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

        free(table);
    }

    /* Free global table */

    for (int i = 0; i < FIELD_HASH_SIZE; i++)
    {
        FieldNode *node = global_table[i];

        while (node != NULL)
        {
            FieldNode *next = node->next;
            free(node);
            node = next;
        }
    }

    free(global_table);
    free(local_tables);
}

typedef struct
{
    int index;
    double value;
} OpenMPJobRank;

static void insert_top10(OpenMPJobRank top[10], int index, double value)
{
    for (int i = 0; i < 10; i++)
    {
        if (value > top[i].value)
        {
            for (int j = 9; j > i; j--)
                top[j] = top[j - 1];

            top[i].index = index;
            top[i].value = value;

            break;
        }
    }
}

void openmp_resource_analysis(Job *jobs, int job_count, int num_threads)
{
    OpenMPJobRank (*local_runtime)[10] =
        calloc(num_threads, sizeof(*local_runtime));

    OpenMPJobRank (*local_queue)[10] =
        calloc(num_threads, sizeof(*local_queue));

    OpenMPJobRank (*local_nodes)[10] =
        calloc(num_threads, sizeof(*local_nodes));

    OpenMPJobRank (*local_cores)[10] =
        calloc(num_threads, sizeof(*local_cores));

    OpenMPJobRank (*local_gpus)[10] =
        calloc(num_threads, sizeof(*local_gpus));

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();

        #pragma omp for
        for (int i = 0; i < job_count; i++)
        {
            insert_top10(local_runtime[tid], i, jobs[i].runtime);
            insert_top10(local_queue[tid], i, jobs[i].queue_wait);
            insert_top10(local_nodes[tid], i, jobs[i].nodes_used);
            insert_top10(local_cores[tid], i, jobs[i].cores_used);
            insert_top10(local_gpus[tid], i, jobs[i].gpus_requested);
        }
    }

    OpenMPJobRank runtime_top[10] = {0};
    OpenMPJobRank queue_top[10] = {0};
    OpenMPJobRank nodes_top[10] = {0};
    OpenMPJobRank cores_top[10] = {0};
    OpenMPJobRank gpus_top[10] = {0};

    for (int t = 0; t < num_threads; t++)
    {
        for (int i = 0; i < 10; i++)
        {
            insert_top10(runtime_top,
                         local_runtime[t][i].index,
                         local_runtime[t][i].value);

            insert_top10(queue_top,
                         local_queue[t][i].index,
                         local_queue[t][i].value);

            insert_top10(nodes_top,
                         local_nodes[t][i].index,
                         local_nodes[t][i].value);

            insert_top10(cores_top,
                         local_cores[t][i].index,
                         local_cores[t][i].value);

            insert_top10(gpus_top,
                         local_gpus[t][i].index,
                         local_gpus[t][i].value);
        }
    }

    printf("\n========== OPENMP TOP 10 LONGEST RUNNING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[runtime_top[i].index];

        printf("%2d. %-20s | Runtime: %.2f sec | Nodes: %.0f | Cores: %.0f\n",
               i + 1,
               job->job_name,
               job->runtime,
               job->nodes_used,
               job->cores_used);
    }

    printf("\n========== OPENMP TOP 10 LONGEST QUEUE WAITS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[queue_top[i].index];

        printf("%2d. %-20s | Queue Wait: %.2f sec | Nodes: %.0f | Cores: %.0f\n",
               i + 1,
               job->job_name,
               job->queue_wait,
               job->nodes_used,
               job->cores_used);
    }

    printf("\n========== OPENMP TOP 10 NODE-USING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[nodes_top[i].index];

        printf("%2d. %-20s | Nodes: %.0f | Cores: %.0f | GPUs: %.0f\n",
               i + 1,
               job->job_name,
               job->nodes_used,
               job->cores_used,
               job->gpus_requested);
    }

    printf("\n========== OPENMP TOP 10 CORE-USING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[cores_top[i].index];

        printf("%2d. %-20s | Cores: %.0f | Nodes: %.0f | GPUs: %.0f\n",
               i + 1,
               job->job_name,
               job->cores_used,
               job->nodes_used,
               job->gpus_requested);
    }

    printf("\n========== OPENMP TOP 10 GPU-REQUESTING JOBS ==========\n");

    for (int i = 0; i < 10; i++)
    {
        Job *job = &jobs[gpus_top[i].index];

        printf("%2d. %-20s | GPUs: %.0f | Nodes: %.0f | Cores: %.0f\n",
               i + 1,
               job->job_name,
               job->gpus_requested,
               job->nodes_used,
               job->cores_used);
    }

    free(local_runtime);
    free(local_queue);
    free(local_nodes);
    free(local_cores);
    free(local_gpus);
}
