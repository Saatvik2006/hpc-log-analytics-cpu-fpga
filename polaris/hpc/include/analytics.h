#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "job.h"

void dataset_summary(Job *jobs, int job_count);
void top_users(Job *jobs, int job_count);
void top_science_fields(Job *jobs, int job_count);
void longest_running_jobs(Job *jobs, int job_count);
void longest_queue_waits(Job *jobs, int job_count);
void largest_resource_jobs(Job *jobs, int job_count);

#endif
