#include <stdio.h>
#include <stdlib.h>

#include "../include/parser.h"
#include "../include/analytics.h"
#include "../include/ip_hash.h"
#include "../include/ip_counter.h"
#include "../include/timer.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <logfile>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        perror("Error opening file");
        return 1;
    }

    Analytics analytics;
    analytics_init(&analytics);

    IPHash table;
    ip_hash_init(&table);

    start_timer();

    char line[2048];
    LogEntry entry;

    while (fgets(line, sizeof(line), file))
    {
        if (parse_log_line(line, &entry))
        {
            analytics_update(&analytics, &entry);

            ip_counter_update(&table, &entry);
        }
    }

    double runtime = stop_timer();

    fclose(file);

    analytics_print(&analytics, runtime);

    printf("\n");
    printf("=========================================\n");
    printf("Top 10 IP Addresses\n");
    printf("=========================================\n");

    ip_hash_print_top(&table, 10);

    ip_hash_destroy(&table);

    return 0;
}
