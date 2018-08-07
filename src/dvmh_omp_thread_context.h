#ifndef DVMH_OMP_THREAD_CONTEXT_H
#define DVMH_OMP_THREAD_CONTEXT_H

#include <stdbool.h>

#include "dvmh_omp_interval.h"

typedef unsigned int uint;

typedef struct _dvmh_omp_thread_context_t {
    dvmh_omp_interval_t *intervals;
    uint *stack;
    uint size;
    uint top;
    int thread_id;
    bool is_tree_built;
} dvmh_omp_thread_context_t;

dvmh_omp_thread_context_t *
dvmh_omp_thread_context_create(uint size, int thread_id);

void
dvmh_omp_thread_context_destroy(
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

bool
dvmh_omp_thread_context_has_active_interval(
        dvmh_omp_thread_context_t *ctx);

size_t
dvmh_omp_thread_context_sizeof(
        dvmh_omp_thread_context_t *ctx);

int
dvmh_omp_thread_context_thread_id(
        dvmh_omp_thread_context_t *ctx);

dvmh_omp_interval_t *
dvmh_omp_thread_context_interval_tree(
        dvmh_omp_thread_context_t *ctx);

dvmh_omp_interval_t *
dvmh_omp_thread_context_get_interval(
        dvmh_omp_thread_context_t *ctx,
        int id);

#endif
