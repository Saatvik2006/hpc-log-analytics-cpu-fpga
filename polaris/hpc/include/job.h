#ifndef JOB_H
#define JOB_H

typedef struct
{
    char job_name[128];

    long username;

    double runtime;
    double queue_wait;

    double nodes_used;
    double nodes_requested;

    double cores_used;
    double cores_requested;

    double gpus_requested;

    int exit_status;

    char science_field[128];

} Job;

#endif
