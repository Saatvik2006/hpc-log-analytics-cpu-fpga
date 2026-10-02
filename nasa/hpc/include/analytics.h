#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "parser.h"

typedef struct
{
    long parsed_lines;

    long get_requests;
    long post_requests;
    long other_requests;

    long response_2xx;
    long response_3xx;
    long response_4xx;
    long response_5xx;

    long total_bytes;

} Analytics;

void analytics_init(Analytics *a);

void analytics_update(Analytics *a,const LogEntry *entry);

void analytics_merge(Analytics *dest,const Analytics *src);

void analytics_print(const Analytics *a,double runtime);

#endif
