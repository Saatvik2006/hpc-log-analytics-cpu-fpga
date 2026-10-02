#include <stdio.h>
#include <string.h>

#include "../include/analytics.h"

void analytics_init(Analytics *a)
{
    memset(a,0,sizeof(Analytics));
}

void analytics_update(Analytics *a,const LogEntry *entry)
{
    a->parsed_lines++;

    if(strcmp(entry->method,"GET")==0)
        a->get_requests++;
    else if(strcmp(entry->method,"POST")==0)
        a->post_requests++;
    else
        a->other_requests++;

    if(entry->status>=200 && entry->status<300)
        a->response_2xx++;
    else if(entry->status>=300 && entry->status<400)
        a->response_3xx++;
    else if(entry->status>=400 && entry->status<500)
        a->response_4xx++;
    else if(entry->status>=500)
        a->response_5xx++;

    a->total_bytes += entry->bytes;
}

void analytics_merge(Analytics *dest,const Analytics *src)
{
    dest->parsed_lines += src->parsed_lines;

    dest->get_requests += src->get_requests;
    dest->post_requests += src->post_requests;
    dest->other_requests += src->other_requests;

    dest->response_2xx += src->response_2xx;
    dest->response_3xx += src->response_3xx;
    dest->response_4xx += src->response_4xx;
    dest->response_5xx += src->response_5xx;

    dest->total_bytes += src->total_bytes;
}

void analytics_print(const Analytics *a,double runtime)
{
    printf("\n=========================================\n");

    printf("Parsed Lines      : %ld\n",a->parsed_lines);
    printf("GET Requests      : %ld\n",a->get_requests);
    printf("POST Requests     : %ld\n",a->post_requests);
    printf("Other Requests    : %ld\n",a->other_requests);

    printf("2xx Responses     : %ld\n",a->response_2xx);
    printf("3xx Responses     : %ld\n",a->response_3xx);
    printf("4xx Responses     : %ld\n",a->response_4xx);
    printf("5xx Responses     : %ld\n",a->response_5xx);

    printf("Total Bytes       : %ld\n",a->total_bytes);

    printf("Runtime           : %.6f sec\n",runtime);

    printf("=========================================\n");
}
