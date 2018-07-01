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

    ctx->locks = (omp_lock_t *)
            malloc(num_context_descriptors * sizeof(omp_lock_t));
    for (int i = 0; i < ctx->num_context_descriptors; ++i) {
        omp_init_lock(ctx->locks + i);
    }

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

    assert(ctx->locks != NULL);
    for (int i = 0; i < ctx->num_context_descriptors; ++i) {
        omp_destroy_lock(ctx->locks + i);
    }
    free(ctx->locks);

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

void dvmh_omp_runtime_context_set_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        context_descriptor *cd,
        int cd_id)
{
    assert(ctx != NULL);
    assert(cd != NULL);
    assert(0 <= cd_id && cd_id < ctx->num_context_descriptors);
    ctx->context_descriptors[cd_id] = cd;
}
