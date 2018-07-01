#include <stdlib.h>
#include <assert.h>

#include "dvmh_omp_runtime_context.h"

dvmh_omp_runtime_context_t *
dvmh_omp_runtime_context_create(
        int num_threads,
        int num_context_descriptors)
{
    assert(num_threads > 0);
    assert(num_context_descriptors > 0);
    dvmh_omp_runtime_context_t *ctx = (dvmh_omp_runtime_context_t *)
            malloc(sizeof(dvmh_omp_runtime_context_t));
    assert(ctx != NULL);

    ctx->num_threads = num_threads;
    ctx->thread_contexts = (dvmh_omp_thread_context **)
            malloc(num_threads * sizeof(dvmh_omp_thread_context *));
    assert(ctx->thread_contexts);

    ctx->num_context_descriptors = num_context_descriptors;
    ctx->context_descriptors = (context_descriptor **)
            malloc(num_context_descriptors * sizeof(context_descriptor *));
    assert(ctx->context_descriptors);

    ctx->interval_locks = (omp_lock_t *)
            malloc(num_context_descriptors * sizeof(omp_lock_t));
    assert(ctx->interval_locks != NULL);
    for (int i = 0; i < ctx->num_context_descriptors; ++i) {
        omp_init_lock(ctx->interval_locks + i);
    }

    ctx->threads_in_interval = (int *)
            calloc(num_context_descriptors, sizeof(int));
    assert(ctx->threads_in_interval);

    ctx->execution_times = (double *)
            calloc(num_context_descriptors, sizeof(double));
    assert(ctx->execution_times);

    ctx->end_parallel_times = (double *)
            calloc(num_threads, sizeof(double));
    assert(ctx->end_parallel_times);

    ctx->idle_parallel_times = (double *)
            calloc(num_context_descriptors, sizeof(double));
    assert(ctx->idle_parallel_times);

    return ctx;
}

void
dvmh_omp_runtime_context_destroy(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);

    assert(ctx->thread_contexts != NULL);
    free(ctx->thread_contexts);

    assert(ctx->context_descriptors != NULL);
    free(ctx->context_descriptors);

    assert(ctx->interval_locks != NULL);
    for (int i = 0; i < ctx->num_context_descriptors; ++i) {
        omp_destroy_lock(ctx->interval_locks + i);
    }
    free(ctx->interval_locks);

    assert(ctx->threads_in_interval);
    free(ctx->threads_in_interval);

    assert(ctx->execution_times);
    free(ctx->execution_times);

    assert(ctx->end_parallel_times);
    free(ctx->end_parallel_times);

    assert(ctx->idle_parallel_times);
    free(ctx->idle_parallel_times);

    free(ctx);
}

void
dvmh_omp_runtime_context_set_thread_context(
        dvmh_omp_runtime_context_t *r_ctx,
        dvmh_omp_thread_context *t_ctx,
        int thread_id)
{
    assert(r_ctx != NULL);
    assert(t_ctx != NULL);
    assert(0 <= thread_id && thread_id < r_ctx->num_threads);
    r_ctx->thread_contexts[thread_id] = t_ctx;
}

void
dvmh_omp_runtime_context_set_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        context_descriptor *cd,
        int cd_id)
{
    assert(ctx != NULL);
    assert(cd != NULL);
    assert(0 <= cd_id && cd_id < ctx->num_context_descriptors);
    ctx->context_descriptors[cd_id] = cd;
}

void
dvmh_omp_runtime_context_lock_interval(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    omp_set_lock(ctx->interval_locks + id);
}

void
dvmh_omp_runtime_context_unlock_interval(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    omp_unset_lock(ctx->interval_locks + id);
}


// must be under lock.
void
dvmh_omp_runtime_context_inc_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->threads_in_interval[id]++;
}

// must be under lock.
int
dvmh_omp_runtime_context_get_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    return ctx->threads_in_interval[id];
}

// must be under lock.
void
dvmh_omp_runtime_context_dec_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->threads_in_interval[id]--;
}

// must be under lock.
void
dvmh_omp_runtime_context_add_exectuion_time(
        dvmh_omp_runtime_context_t *ctx,
        int id,
        double execution_time)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->execution_times[id] += execution_time;
}

void
dvmh_omp_runtime_context_end_parallel(
    dvmh_omp_runtime_context_t *ctx,
    int thread_id,
    double when)
{
    assert(ctx != NULL);
    assert(0 <= thread_id && thread_id < ctx->num_threads);
    ctx->end_parallel_times[thread_id] = when;
}

void
dvmh_omp_runtime_context_after_parallel(
    dvmh_omp_runtime_context_t *ctx,
    int interval_id,
    double when)
{
    assert(ctx != NULL);
    assert(0 <= interval_id && interval_id < ctx->num_context_descriptors);
    for (int i = 0; i < ctx->num_threads; ++i) {
        ctx->idle_parallel_times[interval_id] = when - ctx->end_parallel_times[i];
    }
}
