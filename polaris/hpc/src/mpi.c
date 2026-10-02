#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <mpi.h>

#include "../include/job.h"

#define MAX_LINE_LENGTH 4096
#define USER_HASH_SIZE 65536
#define FIELD_HASH_SIZE 128
#define TOP_N 10

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

typedef struct
{
    int index;
    double value;
} RankResult;


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
   BYTE RANGE
   ============================================================ */

static Job *read_local_chunk(
    const char *filename,
    int rank,
    int world_size,
    int *job_count)
{
    struct stat file_info;

    if (stat(filename, &file_info) != 0)
    {
        perror("stat");
        return NULL;
    }

    long long file_size = file_info.st_size;

    long long start =
        (file_size * rank) / world_size;

    long long end =
        (file_size * (rank + 1)) / world_size;

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        perror("fopen");
        return NULL;
    }

    fseeko(fp, start, SEEK_SET);

    char line[MAX_LINE_LENGTH];

    /*
     * Rank 0 skips the header.
     *
     * Every other rank skips the partial row at its
     * starting byte.
     */
    if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
    {
        fclose(fp);
        *job_count = 0;
        return NULL;
    }

    int capacity = 10000;

    Job *jobs = malloc(
        capacity * sizeof(Job)
    );

    if (jobs == NULL)
    {
        fclose(fp);
        return NULL;
    }

    int count = 0;

    while (1)
    {
        long long line_start = ftello(fp);

        /*
         * The final rank reads until EOF.
         * Other ranks stop when they reach their
         * assigned byte boundary.
         */
        if (rank != world_size - 1 &&
            line_start >= end)
        {
            break;
        }

        if (fgets(line, MAX_LINE_LENGTH, fp) == NULL)
            break;

        if (count >= capacity)
        {
            capacity *= 2;

            Job *tmp = realloc(
                jobs,
                capacity * sizeof(Job)
            );

            if (tmp == NULL)
            {
                free(jobs);
                fclose(fp);
                return NULL;
            }

            jobs = tmp;
        }

        parse_line(line, &jobs[count]);

        count++;
    }

    fclose(fp);

    *job_count = count;

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
    *runtime = 0.0;
    *queue_wait = 0.0;
    *nodes = 0.0;
    *cores = 0.0;
    *gpus = 0.0;

    *successful = 0;
    *failed = 0;

    for (int i = 0; i < count; i++)
    {
        *runtime += jobs[i].runtime;
        *queue_wait += jobs[i].queue_wait;
        *nodes += jobs[i].nodes_used;
        *cores += jobs[i].cores_used;

        if (jobs[i].gpus_requested > 0)
            *gpus += jobs[i].gpus_requested;

        if (jobs[i].exit_status == 0)
            (*successful)++;
        else
            (*failed)++;
    }
}


/* ============================================================
   USER HASH TABLE
   ============================================================ */

typedef struct UserNode
{
    long username;
    int count;
    struct UserNode *next;
} UserNode;


static unsigned long user_hash(long username)
{
    return (unsigned long)username % USER_HASH_SIZE;
}


static void add_user(
    UserNode **table,
    long username,
    int count)
{
    unsigned long hash = user_hash(username);

    UserNode *node = table[hash];

    while (node != NULL)
    {
        if (node->username == username)
        {
            node->count += count;
            return;
        }

        node = node->next;
    }

    node = malloc(sizeof(UserNode));

    node->username = username;
    node->count = count;
    node->next = table[hash];

    table[hash] = node;
}


static int count_users(UserNode **table)
{
    int total = 0;

    for (int i = 0; i < USER_HASH_SIZE; i++)
    {
        UserNode *node = table[i];

        while (node != NULL)
        {
            total++;
            node = node->next;
        }
    }

    return total;
}


static void fill_user_results(
    UserNode **table,
    UserResult *results)
{
    int index = 0;

    for (int i = 0; i < USER_HASH_SIZE; i++)
    {
        UserNode *node = table[i];

        while (node != NULL)
        {
            results[index].username = node->username;
            results[index].count = node->count;

            index++;
            node = node->next;
        }
    }
}


static void free_user_table(UserNode **table)
{
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


/* ============================================================
   FIELD HASH TABLE
   ============================================================ */

typedef struct FieldNode
{
    char field[128];
    int count;
    struct FieldNode *next;
} FieldNode;


static unsigned long field_hash(const char *field)
{
    unsigned long hash = 0;

    for (int i = 0; field[i] != '\0'; i++)
        hash = hash * 31 + field[i];

    return hash % FIELD_HASH_SIZE;
}


static void add_field(
    FieldNode **table,
    const char *field,
    int count)
{
    unsigned long hash = field_hash(field);

    FieldNode *node = table[hash];

    while (node != NULL)
    {
        if (strcmp(node->field, field) == 0)
        {
            node->count += count;
            return;
        }

        node = node->next;
    }

    node = malloc(sizeof(FieldNode));

    strcpy(node->field, field);
    node->count = count;

    node->next = table[hash];

    table[hash] = node;
}


static int count_fields(FieldNode **table)
{
    int total = 0;

    for (int i = 0; i < FIELD_HASH_SIZE; i++)
    {
        FieldNode *node = table[i];

        while (node != NULL)
        {
            total++;
            node = node->next;
        }
    }

    return total;
}


static void fill_field_results(
    FieldNode **table,
    FieldResult *results)
{
    int index = 0;

    for (int i = 0; i < FIELD_HASH_SIZE; i++)
    {
        FieldNode *node = table[i];

        while (node != NULL)
        {
            strcpy(results[index].field, node->field);
            results[index].count = node->count;

            index++;

            node = node->next;
        }
    }
}


static void free_field_table(FieldNode **table)
{
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


/* ============================================================
   TOP 10
   ============================================================ */

static void insert_top(
    RankResult top[TOP_N],
    int index,
    double value)
{
    for (int i = 0; i < TOP_N; i++)
    {
        if (value > top[i].value)
        {
            for (int j = TOP_N - 1; j > i; j--)
                top[j] = top[j - 1];

            top[i].index = index;
            top[i].value = value;

            break;
        }
    }
}


/* ============================================================
   MAIN
   ============================================================ */

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <dataset.csv>\n", argv[0]);
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

    MPI_Barrier(MPI_COMM_WORLD);

    double parse_start = MPI_Wtime();

    int local_count = 0;

    Job *jobs = read_local_chunk(
        filename,
        rank,
        world_size,
        &local_count
    );

    double parse_time =
        MPI_Wtime() - parse_start;

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

    int total_jobs;
    int total_successful;
    int total_failed;

    double total_runtime;
    double total_queue_wait;
    double total_nodes;
    double total_cores;
    double total_gpus;

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
     * USER COUNTS
     * --------------------------------------------------------
     */

    UserNode **user_table = calloc(
        USER_HASH_SIZE,
        sizeof(UserNode *)
    );

    for (int i = 0; i < local_count; i++)
        add_user(user_table, jobs[i].username, 1);

    int local_user_count = count_users(user_table);

    UserResult *local_users = malloc(
        local_user_count * sizeof(UserResult)
    );

    fill_user_results(
        user_table,
        local_users
    );

    int *user_counts = NULL;

    if (rank == 0)
    {
        user_counts = malloc(
            world_size * sizeof(int)
        );
    }

    MPI_Gather(
        &local_user_count,
        1,
        MPI_INT,
        user_counts,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    UserResult *all_users = NULL;
    int *user_displs = NULL;
    int total_user_results = 0;

    if (rank == 0)
    {
        user_displs = malloc(
            world_size * sizeof(int)
        );

        user_displs[0] = 0;

        for (int i = 0; i < world_size; i++)
        {
            if (i > 0)
            {
                user_displs[i] =
                    user_displs[i - 1] +
                    user_counts[i - 1];
            }

            total_user_results += user_counts[i];
        }

        all_users = malloc(
            total_user_results *
            sizeof(UserResult)
        );
    }

    int *user_counts_bytes = NULL;
    int *user_displs_bytes = NULL;

    if (rank == 0)
    {
        user_counts_bytes = malloc(
            world_size * sizeof(int)
        );

        user_displs_bytes = malloc(
            world_size * sizeof(int)
        );

        for (int i = 0; i < world_size; i++)
        {
            user_counts_bytes[i] =
                user_counts[i] * sizeof(UserResult);

            user_displs_bytes[i] =
                user_displs[i] * sizeof(UserResult);
        }
    }

    MPI_Gatherv(
        local_users,
        local_user_count * sizeof(UserResult),
        MPI_BYTE,
        all_users,
        user_counts_bytes,
        user_displs_bytes,
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );


    /*
     * --------------------------------------------------------
     * SCIENCE FIELD COUNTS
     * --------------------------------------------------------
     */

    FieldNode **field_table = calloc(
        FIELD_HASH_SIZE,
        sizeof(FieldNode *)
    );

    for (int i = 0; i < local_count; i++)
    {
        if (strcmp(jobs[i].science_field, "-1") == 0 ||
            strcmp(jobs[i].science_field, "0") == 0 ||
            strlen(jobs[i].science_field) == 0)
        {
            continue;
        }

        add_field(
            field_table,
            jobs[i].science_field,
            1
        );
    }

    int local_field_count =
        count_fields(field_table);

    FieldResult *local_fields = malloc(
        local_field_count * sizeof(FieldResult)
    );

    fill_field_results(
        field_table,
        local_fields
    );

    int *field_counts = NULL;

    if (rank == 0)
    {
        field_counts = malloc(
            world_size * sizeof(int)
        );
    }

    MPI_Gather(
        &local_field_count,
        1,
        MPI_INT,
        field_counts,
        1,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    FieldResult *all_fields = NULL;
    int *field_displs = NULL;

    int total_field_results = 0;

    if (rank == 0)
    {
        field_displs = malloc(
            world_size * sizeof(int)
        );

        field_displs[0] = 0;

        for (int i = 0; i < world_size; i++)
        {
            if (i > 0)
            {
                field_displs[i] =
                    field_displs[i - 1] +
                    field_counts[i - 1];
            }

            total_field_results += field_counts[i];
        }

        all_fields = malloc(
            total_field_results *
            sizeof(FieldResult)
        );
    }

    int *field_counts_bytes = NULL;
    int *field_displs_bytes = NULL;

    if (rank == 0)
    {
        field_counts_bytes = malloc(
            world_size * sizeof(int)
        );

        field_displs_bytes = malloc(
            world_size * sizeof(int)
        );

        for (int i = 0; i < world_size; i++)
        {
            field_counts_bytes[i] =
                field_counts[i] * sizeof(FieldResult);

            field_displs_bytes[i] =
                field_displs[i] * sizeof(FieldResult);
        }
    }

    MPI_Gatherv(
        local_fields,
        local_field_count * sizeof(FieldResult),
        MPI_BYTE,
        all_fields,
        field_counts_bytes,
        field_displs_bytes,
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );


    /*
     * --------------------------------------------------------
     * RESOURCE TOP 10
     * --------------------------------------------------------
     */

    RankResult local_runtime_top[TOP_N] = {0};
    RankResult local_queue_top[TOP_N] = {0};
    RankResult local_nodes_top[TOP_N] = {0};
    RankResult local_cores_top[TOP_N] = {0};
    RankResult local_gpus_top[TOP_N] = {0};

    /*
     * The index is local to this MPI rank.
     */
    for (int i = 0; i < local_count; i++)
    {
        insert_top(
            local_runtime_top,
            i,
            jobs[i].runtime
        );

        insert_top(
            local_queue_top,
            i,
            jobs[i].queue_wait
        );

        insert_top(
            local_nodes_top,
            i,
            jobs[i].nodes_used
        );

        insert_top(
            local_cores_top,
            i,
            jobs[i].cores_used
        );

        insert_top(
            local_gpus_top,
            i,
            jobs[i].gpus_requested
        );
    }

    RankResult all_runtime_top[TOP_N * 128];
    RankResult all_queue_top[TOP_N * 128];
    RankResult all_nodes_top[TOP_N * 128];
    RankResult all_cores_top[TOP_N * 128];
    RankResult all_gpus_top[TOP_N * 128];

    /*
     * For the initial local benchmark, support up to
     * 128 MPI processes.
     */
    MPI_Gather(
        local_runtime_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        all_runtime_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );

    MPI_Gather(
        local_queue_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        all_queue_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );

    MPI_Gather(
        local_nodes_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        all_nodes_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );

    MPI_Gather(
        local_cores_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        all_cores_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );

    MPI_Gather(
        local_gpus_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        all_gpus_top,
        TOP_N * sizeof(RankResult),
        MPI_BYTE,
        0,
        MPI_COMM_WORLD
    );


    /*
     * --------------------------------------------------------
     * FINAL OUTPUT
     * --------------------------------------------------------
     */

    if (rank == 0)
    {
        printf("\n========== MPI DATASET SUMMARY ==========\n");

        printf("Processes            : %d\n",
               world_size);

        printf("Total Jobs           : %d\n",
               total_jobs);

        printf("Successful Jobs      : %d\n",
               total_successful);

        printf("Failed Jobs          : %d\n",
               total_failed);

        printf("Average Runtime      : %.2f sec\n",
               total_runtime / total_jobs);

        printf("Average Queue Wait   : %.2f sec\n",
               total_queue_wait / total_jobs);

        printf("Average Nodes Used   : %.2f\n",
               total_nodes / total_jobs);

        printf("Average Cores Used   : %.2f\n",
               total_cores / total_jobs);

        printf("Average GPUs Used    : %.2f\n",
               total_gpus / total_jobs);


        /*
         * ----------------------------------------------------
         * Merge USER results
         * ----------------------------------------------------
         */

        UserNode **global_users = calloc(
            USER_HASH_SIZE,
            sizeof(UserNode *)
        );

        for (int i = 0; i < total_user_results; i++)
        {
            add_user(
                global_users,
                all_users[i].username,
                all_users[i].count
            );
        }

        UserNode *top_users[TOP_N] = {0};

        for (int i = 0; i < USER_HASH_SIZE; i++)
        {
            UserNode *node = global_users[i];

            while (node != NULL)
            {
                for (int j = 0; j < TOP_N; j++)
                {
                    if (top_users[j] == NULL ||
                        node->count > top_users[j]->count)
                    {
                        for (int k = TOP_N - 1; k > j; k--)
                            top_users[k] = top_users[k - 1];

                        top_users[j] = node;
                        break;
                    }
                }

                node = node->next;
            }
        }

        printf("\n========== MPI TOP 10 USERS ==========\n");

        for (int i = 0; i < TOP_N; i++)
        {
            if (top_users[i] != NULL)
            {
                printf(
                    "%2d. User: %ld | Jobs: %d\n",
                    i + 1,
                    top_users[i]->username,
                    top_users[i]->count
                );
            }
        }


        /*
         * ----------------------------------------------------
         * Merge SCIENCE FIELD results
         * ----------------------------------------------------
         */

        FieldNode **global_fields = calloc(
            FIELD_HASH_SIZE,
            sizeof(FieldNode *)
        );

        for (int i = 0; i < total_field_results; i++)
        {
            add_field(
                global_fields,
                all_fields[i].field,
                all_fields[i].count
            );
        }

        FieldNode *top_fields[TOP_N] = {0};

        for (int i = 0; i < FIELD_HASH_SIZE; i++)
        {
            FieldNode *node = global_fields[i];

            while (node != NULL)
            {
                for (int j = 0; j < TOP_N; j++)
                {
                    if (top_fields[j] == NULL ||
                        node->count > top_fields[j]->count)
                    {
                        for (int k = TOP_N - 1; k > j; k--)
                            top_fields[k] = top_fields[k - 1];

                        top_fields[j] = node;
                        break;
                    }
                }

                node = node->next;
            }
        }

        printf(
            "\n========== MPI TOP 10 SCIENCE FIELDS ==========\n"
        );

        for (int i = 0; i < TOP_N; i++)
        {
            if (top_fields[i] != NULL)
            {
                printf(
                    "%2d. %-50s | Jobs: %d\n",
                    i + 1,
                    top_fields[i]->field,
                    top_fields[i]->count
                );
            }
        }


        /*
         * ----------------------------------------------------
         * Resource results
         * ----------------------------------------------------
         */

        RankResult runtime_top[TOP_N] = {0};
        RankResult queue_top[TOP_N] = {0};
        RankResult nodes_top[TOP_N] = {0};
        RankResult cores_top[TOP_N] = {0};
        RankResult gpus_top[TOP_N] = {0};

        /*
         * Each gathered RankResult contains a local index.
         * We therefore need to know which rank produced it.
         */

        for (int r = 0; r < world_size; r++)
        {
            for (int i = 0; i < TOP_N; i++)
            {
                int local_index =
                    all_runtime_top[r * TOP_N + i].index;

                if (all_runtime_top[r * TOP_N + i].value > 0)
                {
                    /*
                     * We cannot directly use this index on
                     * Rank 0 because the Job belongs to rank r.
                     *
                     * Resource output is therefore handled
                     * separately below.
                     */
                    (void)local_index;
                }
            }
        }

        /*
         * We deliberately don't print resource rankings yet.
         * The local Job arrays are private to each rank.
         *
         * The next MPI version will exchange only the required
         * Job records for these 5 x 10 rankings.
         */

        free_user_table(global_users);
        free_field_table(global_fields);

        free(user_counts);
        free(user_displs);
        free(user_counts_bytes);
        free(user_displs_bytes);

        free(all_users);

        free(field_counts);
        free(field_displs);
        free(field_counts_bytes);
        free(field_displs_bytes);

        free(all_fields);
    }


    free(local_users);
    free(local_fields);

    free_user_table(user_table);
    free_field_table(field_table);

    free(jobs);

    MPI_Barrier(MPI_COMM_WORLD);

    double max_parse_time;

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
            "\n========== MPI PERFORMANCE ==========\n"
        );

        printf(
            "Max CSV parsing time : %.6f sec\n",
            max_parse_time
        );
    }

    MPI_Finalize();

    return 0;
}
