#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/ip_hash.h"

static unsigned int hash_function(const char *str)
{
    unsigned int hash = 5381;

    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash % HASH_SIZE;
}

void ip_hash_init(IPHash *table)
{
    for (int i = 0; i < HASH_SIZE; i++)
        table->buckets[i] = NULL;
}

void ip_hash_insert(IPHash *table, const char *ip)
{
    unsigned int index = hash_function(ip);

    IPNode *current = table->buckets[index];

    while (current != NULL)
    {
        if (strcmp(current->ip, ip) == 0)
        {
            current->count++;
            return;
        }

        current = current->next;
    }

    IPNode *node = (IPNode *)malloc(sizeof(IPNode));

    if (node == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(node->ip, ip);

    node->count = 1;

    node->next = table->buckets[index];

    table->buckets[index] = node;
}

void ip_hash_merge(IPHash *dest, const IPHash *src)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        IPNode *current = src->buckets[i];

        while (current != NULL)
        {
            unsigned int index = hash_function(current->ip);

            IPNode *d = dest->buckets[index];

            while (d != NULL)
            {
                if (strcmp(d->ip, current->ip) == 0)
                {
                    d->count += current->count;
                    break;
                }

                d = d->next;
            }

            if (d == NULL)
            {
                IPNode *node = (IPNode *)malloc(sizeof(IPNode));

                if (node == NULL)
                {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }

                strcpy(node->ip, current->ip);

                node->count = current->count;

                node->next = dest->buckets[index];

                dest->buckets[index] = node;
            }

            current = current->next;
        }
    }
}

static int compare_desc(const void *a, const void *b)
{
    const IPResult *x = (const IPResult *)a;
    const IPResult *y = (const IPResult *)b;

    if (x->count < y->count)
        return 1;

    if (x->count > y->count)
        return -1;

    return 0;
}

void ip_hash_print_top(IPHash *table, int top_n)
{
    int total = 0;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        IPNode *current = table->buckets[i];

        while (current != NULL)
        {
            total++;
            current = current->next;
        }
    }

    if (total == 0)
        return;

    IPResult *results = (IPResult *)malloc(total * sizeof(IPResult));

    if (results == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int idx = 0;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        IPNode *current = table->buckets[i];

        while (current != NULL)
        {
            strcpy(results[idx].ip, current->ip);
            results[idx].count = current->count;

            idx++;

            current = current->next;
        }
    }

    qsort(results, total, sizeof(IPResult), compare_desc);

    if (top_n > total)
        top_n = total;

    printf("\n");
    printf("========== Top %d IP Addresses ==========\n", top_n);

    for (int i = 0; i < top_n; i++)
    {
        printf("%2d. %-25s %10ld requests\n",
               i + 1,
               results[i].ip,
               results[i].count);
    }

    printf("=========================================\n");

    free(results);
}

void ip_hash_destroy(IPHash *table)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        IPNode *current = table->buckets[i];

        while (current != NULL)
        {
            IPNode *next = current->next;

            free(current);

            current = next;
        }

        table->buckets[i] = NULL;
    }
}

int ip_hash_count_entries(const IPHash *table)
{
    int total = 0;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        IPNode *current = table->buckets[i];

        while (current)
        {
            total++;
            current = current->next;
        }
    }

    return total;
}

void ip_hash_export(const IPHash *table,
                    IPResult *results)
{
    int idx = 0;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        IPNode *current = table->buckets[i];

        while (current)
        {
            strcpy(results[idx].ip, current->ip);
            results[idx].count = current->count;

            idx++;

            current = current->next;
        }
    }
}

void ip_hash_insert_count(IPHash *table,
                          const char *ip,
                          long count)
{
    unsigned int index = hash_function(ip);

    IPNode *current = table->buckets[index];

    while (current)
    {
        if (strcmp(current->ip, ip) == 0)
        {
            current->count += count;
            return;
        }

        current = current->next;
    }

    IPNode *node = malloc(sizeof(IPNode));

    if (!node)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(node->ip, ip);

    node->count = count;

    node->next = table->buckets[index];

    table->buckets[index] = node;
}
