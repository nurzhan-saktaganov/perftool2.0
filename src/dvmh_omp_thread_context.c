#include <assert.h>
#include <stdlib.h>

#include "dvmh_omp_interval.h"
#include "dvmh_omp_thread_context.h"

dvmh_omp_thread_context_t *
dvmh_omp_thread_context_create(uint size, int thread_id)
{
    assert(size != 0);
    dvmh_omp_thread_context_t *ctx = (dvmh_omp_thread_context_t *) malloc(sizeof(dvmh_omp_thread_context_t));
    assert(ctx != NULL);
    ctx->intervals = (dvmh_omp_interval_t *) malloc(sizeof(dvmh_omp_interval_t) * size);
    assert(ctx->intervals != NULL);
    ctx->stack = (uint *) malloc(sizeof(uint) * size);
    assert(ctx->stack != NULL);
    ctx->size = size;
    ctx->top = 0;
    ctx->thread_id = thread_id;
    return ctx;
}

void
dvmh_omp_thread_context_destroy(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->size != 0);
    assert(ctx->intervals != NULL);
    free(ctx->intervals);
    assert(ctx->stack != NULL);
    free(ctx->stack);
    free(ctx);
}

void
dvmh_omp_thread_context_enter_interval(
       dvmh_omp_thread_context_t *ctx, uint id)
{
    assert(0 <= id && id < ctx->size);
    assert(ctx->top < ctx->size);
    ctx->stack[ctx->top] = id;
    ++ctx->top;
}

void
dvmh_omp_thread_context_leave_interval(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->top > 0);
    --ctx->top;
}

dvmh_omp_interval_t *
dvmh_omp_thread_context_current_interval(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->top > 0);
    uint id = ctx->stack[ctx->top - 1];
    return &ctx->intervals[id];
}

size_t
dvmh_omp_thread_context_sizeof(
        dvmh_omp_thread_context_t *ctx)
{
    if (ctx == NULL) return 0;

    return sizeof(dvmh_omp_thread_context_t)
        + ctx->size * sizeof(dvmh_omp_interval_t)
        + ctx->size * sizeof(uint);
}

int
dvmh_omp_thread_context_thread_id(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    return ctx->thread_id;
}
