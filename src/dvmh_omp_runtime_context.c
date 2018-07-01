#include <stdlib.h>
#include <assert.h>

#include "dvmh_omp_runtime_context.h"

dvmh_omp_runtime_context_t *
dvmh_omp_runtime_context_create(
        int num_threads)
{
    assert(num_threads > 0);
    dvmh_omp_runtime_context_t *ctx = (dvmh_omp_runtime_context_t *)
            malloc(sizeof(dvmh_omp_runtime_context_t));
    assert(ctx != NULL);
    ctx->num_threads = num_threads;
    ctx->thread_contexts = (dvmh_omp_thread_context **)
            malloc(num_threads * sizeof(dvmh_omp_thread_context *));
    assert(ctx->thread_contexts);
    return ctx;
}

void
dvmh_omp_runtime_context_destroy(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->thread_contexts != NULL);
    free(ctx->thread_contexts);
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
