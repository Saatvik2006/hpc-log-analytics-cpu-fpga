#include <time.h>

static double start;

double get_time()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void start_timer()
{
    start = get_time();
}

double stop_timer()
{
    return get_time() - start;
}
