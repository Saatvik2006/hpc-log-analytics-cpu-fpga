#ifndef IP_COUNTER_H
#define IP_COUNTER_H

#include "parser.h"
#include "ip_hash.h"

void ip_counter_update(IPHash *table, const LogEntry *entry);

#endif
