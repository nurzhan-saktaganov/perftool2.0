#include <assert.h>
#include <stdbool.h>
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
    for (int i = 0; i < size; ++i) {
        dvmh_omp_interval_init(&ctx->intervals[i]);
        dvmh_omp_interval_set_id(&ctx->intervals[i], i);
    }
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
    for (int i = 0; i < ctx->size; ++i) {
        dvmh_omp_interval_deinit(&ctx->intervals[i]);
    }
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

bool
dvmh_omp_thread_context_has_active_interval(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    return ctx->top > 0;
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

dvmh_omp_interval_t *
dvmh_omp_thread_context_interval_tree(
        dvmh_omp_thread_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->size > 0);
    assert(ctx->intervals != NULL);

    if ( ctx->is_tree_built ) {
        return &ctx->intervals[0];
    }

    // We assume that interval at the zero position represents the whole program.
    // We also assume that interval at position i occurs in code source before interval at position j if i < j.
    for (int i = 1; i < ctx->size; ++i) {
        dvmh_omp_interval_t *current = &ctx->intervals[i];
        const int parent_id = dvmh_omp_interval_get_parent_id(current);
        assert(0 <= parent_id && parent_id < ctx->size);
        dvmh_omp_interval_t *parent = &ctx->intervals[parent_id];
        dvmh_omp_interval_add_subinterval(parent, current);
    }

    ctx->is_tree_built = true;

    return &ctx->intervals[0];
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
