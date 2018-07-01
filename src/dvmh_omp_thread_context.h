#ifndef DVMH_OMP_THREAD_CONTEXT_H
#define DVMH_OMP_THREAD_CONTEXT_H

typedef unsigned int uint;

#include "dvmh_omp_interval.h"

typedef struct _dvmh_omp_thread_context {
    dvmh_omp_interval *intervals;
    uint *stack;
    uint size;
    uint top;
    int thread_id;
} dvmh_omp_thread_context;

dvmh_omp_thread_context *
dvmh_omp_thread_context_create(uint size, int thread_id);

void
dvmh_omp_thread_context_destroy(
        dvmh_omp_thread_context *ctx);

void
dvmh_omp_thread_context_enter_interval(
       dvmh_omp_thread_context *ctx, uint id);

void
dvmh_omp_thread_context_leave_interval(
        dvmh_omp_thread_context *ctx);

dvmh_omp_interval *
dvmh_omp_thread_context_current_interval(
        dvmh_omp_thread_context *ctx);

size_t
dvmh_omp_thread_context_sizeof(
        dvmh_omp_thread_context *ctx);

int
dvmh_omp_thread_context_thread_id(
        dvmh_omp_thread_context *ctx);

#endif
