#ifndef IP_HASH_H
#define IP_HASH_H

#define HASH_SIZE 131071

typedef struct IPNode
{
    char ip[32];
    long count;
    struct IPNode *next;

} IPNode;

typedef struct
{
    IPNode *buckets[HASH_SIZE];

} IPHash;

typedef struct
{
    char ip[32];
    long count;

} IPResult;

void ip_hash_init(IPHash *table);

void ip_hash_insert(IPHash *table, const char *ip);

void ip_hash_merge(IPHash *dest, const IPHash *src);

void ip_hash_print_top(IPHash *table, int top_n);

void ip_hash_destroy(IPHash *table);

int ip_hash_count_entries(const IPHash *table);

void ip_hash_export(const IPHash *table,
                    IPResult *results);

void ip_hash_insert_count(IPHash *table,
                          const char *ip,
                          long count);

#endif
