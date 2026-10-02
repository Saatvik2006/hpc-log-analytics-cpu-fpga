#ifndef PARSER_H
#define PARSER_H

#define MAX_IP_LENGTH 32
#define MAX_METHOD_LENGTH 16

typedef struct
{
    char ip[MAX_IP_LENGTH];
    char method[MAX_METHOD_LENGTH];
    int status;
    long bytes;
} LogEntry;

int parse_log_line(const char *line, LogEntry *entry);

#endif
