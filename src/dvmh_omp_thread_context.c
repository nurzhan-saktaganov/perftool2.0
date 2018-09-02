#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "dvmh_omp_interval.h"
#include "dvmh_omp_thread_context.h"

void
dvmh_omp_thread_context_init(
        dvmh_omp_thread_context_t *ctx,
        uint size,
        int thread_id)
{
    assert(ctx != NULL);
    assert(size != 0);
    assert(thread_id >= 0);

    ctx->intervals = (dvmh_omp_interval_t *) malloc(size * sizeof(dvmh_omp_interval_t));
    assert(ctx->intervals != NULL);

    for (int i = 0; i < size; ++i) {
        dvmh_omp_interval_init(&ctx->intervals[i]);
        dvmh_omp_interval_set_id(&ctx->intervals[i], i);
    }

    ctx->stack = (uint *) malloc(size * sizeof(uint));
    assert(ctx->stack != NULL);

    ctx->top = -1;
    ctx->thread_id = thread_id;
}

void
dvmh_omp_thread_context_deinit(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->intervals != NULL);
    assert(ctx->stack != NULL);

    for (int i = 0; i < ctx->size; ++i) {
        dvmh_omp_interval_deinit(&ctx->intervals[i]);
    }

    free(ctx->intervals);
    ctx->intervals = NULL;

    free(ctx->stack);
    ctx->stack = NULL;
}

void
dvmh_omp_thread_context_enter_interval(
       dvmh_omp_thread_context_t *ctx, uint id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->size);

    ++ctx->top;

    assert(ctx->top < ctx->size);
    ctx->stack[ctx->top] = id;
}

void
dvmh_omp_thread_context_leave_interval(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->top >= 0);
    --ctx->top;
}

dvmh_omp_interval_t *
dvmh_omp_thread_context_current_interval(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->top >= 0);
    const uint id = ctx->stack[ctx->top];
    return &ctx->intervals[id];
}

int
dvmh_omp_thread_context_thread_id(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    return ctx->thread_id;
}

dvmh_omp_interval_t *
dvmh_omp_thread_context_get_interval(
        dvmh_omp_thread_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->size);
    return &ctx->intervals[id];
}
