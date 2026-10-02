#include <stdio.h>
#include <string.h>

#include "../include/parser.h"

int parse_log_line(const char *line, LogEntry *entry)
{
    char timestamp[128];
    char url[512];
    char protocol[64];

    int fields = sscanf(
        line,
        "%31s - - [%127[^]]] \"%15s %511s %63[^\"]\" %d %ld",
        entry->ip,
        timestamp,
        entry->method,
        url,
        protocol,
        &entry->status,
        &entry->bytes
    );

    return fields == 7;
}
