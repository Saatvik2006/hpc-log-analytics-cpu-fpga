#ifndef OPENMP_ANALYTICS_H
#define OPENMP_ANALYTICS_H

#include "job.h"

void openmp_analysis(Job *jobs, int job_count, int num_threads);
void openmp_top_users(Job *jobs, int job_count, int num_threads);
void openmp_top_science_fields(Job *jobs, int job_count, int num_threads);
void openmp_resource_analysis(Job *jobs, int job_count, int num_threads);

#endif
