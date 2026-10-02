#ifndef CSV_READER_H
#define CSV_READER_H

#include "job.h"

Job *read_csv(const char *filename, int *job_count);

Job *read_csv_parallel(const char *filename,
                       int *job_count,
                       int num_threads);

#endif
