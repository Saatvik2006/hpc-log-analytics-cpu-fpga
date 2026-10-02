#include "../include/ip_counter.h"

void ip_counter_update(IPHash *table, const LogEntry *entry)
{
    ip_hash_insert(table, entry->ip);
}
