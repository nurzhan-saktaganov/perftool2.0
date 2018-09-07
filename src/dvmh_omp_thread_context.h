#ifndef DVMH_OMP_THREAD_CONTEXT_H
#define DVMH_OMP_THREAD_CONTEXT_H

#include <stdbool.h>

#include "dvmh_omp_interval.h"

typedef unsigned int uint;

typedef struct _dvmh_omp_thread_context_t {
    // buffer to collect thread local metrics
    dvmh_omp_interval_t *intervals;
    // intervals call stack
    uint *stack;
    // count of intervals
    uint size;
    // pointer to the top of stack
    int top;
    int thread_id;
} dvmh_omp_thread_context_t;

void
dvmh_omp_thread_context_init(
        dvmh_omp_thread_context_t *ctx,
        uint size,
        int thread_id);

void
dvmh_omp_thread_context_deinit(
        dvmh_omp_thread_context_t *ctx);

void
dvmh_omp_thread_context_enter_interval(
       dvmh_omp_thread_context_t *ctx, uint id);

void
dvmh_omp_thread_context_leave_interval(
        dvmh_omp_thread_context_t *ctx);

dvmh_omp_interval_t *
dvmh_omp_thread_context_current_interval(
        dvmh_omp_thread_context_t *ctx);

int
dvmh_omp_thread_context_thread_id(
        dvmh_omp_thread_context_t *ctx);

dvmh_omp_interval_t *
dvmh_omp_thread_context_get_interval(
        dvmh_omp_thread_context_t *ctx,
        int id);

#endif
