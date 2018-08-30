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

    ctx->threads_spawner_interval_id = -1;

    ctx->num_threads = num_threads;
    ctx->thread_contexts = (dvmh_omp_thread_context_t **)
            calloc(num_threads, sizeof(dvmh_omp_thread_context_t *));
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
        dvmh_omp_thread_context_t *t_ctx,
        int thread_id)
{
    assert(r_ctx != NULL);
    assert(t_ctx != NULL);
    assert(0 <= thread_id && thread_id < r_ctx->num_threads);
    r_ctx->thread_contexts[thread_id] = t_ctx;
}

dvmh_omp_thread_context_t *
dvmh_omp_runtime_context_get_thread_context(
        dvmh_omp_runtime_context_t *r_ctx,
        int thread_id)
{
    assert(r_ctx != NULL);
    assert(0 <= thread_id && thread_id < r_ctx->num_threads);
    assert(r_ctx->thread_contexts[thread_id] != NULL);
    return r_ctx->thread_contexts[thread_id];
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
dvmh_omp_runtime_context_set_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->threads_spawner_interval_id = id;
}

int
dvmh_omp_runtime_context_get_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    return ctx->threads_spawner_interval_id;
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

static dvmh_omp_interval_t *
dvmh_omp_runtime_context_build_interval_tree(
        dvmh_omp_runtime_context_t *r_ctx)
{
    dvmh_omp_interval_t *tree;
    assert(r_ctx != NULL);

    // number of intervals is no more than context descriptors
    int *parents = (int *) malloc(r_ctx->num_context_descriptors * sizeof(int));
    assert(parents != NULL);

    for (int i = 1; i < r_ctx->num_context_descriptors; ++i ) {
        parents[i] = DVMH_OMP_INTERVAL_PARENT_UNDEFINED;
    }

    for (int tid = 0; tid < r_ctx->num_threads; ++tid) {
        dvmh_omp_thread_context_t *t_ctx = dvmh_omp_runtime_context_get_thread_context(r_ctx, tid);
        for (int id = 0; id < r_ctx->num_context_descriptors; ++id) {
            dvmh_omp_interval_t *i = dvmh_omp_thread_context_get_interval(t_ctx, id);

            // TODO
            if (dvmh_omp_interval_execution_count(i) == 0)
                continue;

            int parent_id = dvmh_omp_interval_get_parent_id(i);
            assert(parent_id != DVMH_OMP_INTERVAL_PARENT_UNDEFINED);

            if (parents[id] == DVMH_OMP_INTERVAL_PARENT_UNDEFINED)
                parents[id] = parent_id;
            assert(parents[id] == parent_id);
        }
    }

    tree = (dvmh_omp_interval_t *) malloc(r_ctx->num_context_descriptors * sizeof(dvmh_omp_interval_t));
    assert(tree != NULL);

    for(int id = 0; id < r_ctx->num_context_descriptors; ++id) {
        dvmh_omp_interval_t *i = &tree[id];
        dvmh_omp_interval_init(i);
        dvmh_omp_interval_set_id(i, id);
    }

    // interval with id 0 has not parent
    for (int id = 1; id < r_ctx->num_context_descriptors; ++id) {
        const int parent_id = parents[id];
        if (parent_id == DVMH_OMP_INTERVAL_PARENT_UNDEFINED)
            continue;

        dvmh_omp_interval_t *i = &tree[id];
        dvmh_omp_interval_t *p = &tree[parent_id];

        dvmh_omp_interval_add_subinterval(p, i);
    }

    free(parents);

    return tree;
}

static void
dvmh_omp_runtime_context_traverse(
        dvmh_omp_runtime_context_t *r_ctx,
        dvmh_omp_interval_t *node)
{
        assert(r_ctx != NULL);
        assert(node != NULL);

        const int id = dvmh_omp_interval_get_id(node);

        if (dvmh_omp_interval_has_subintervals(node)) {
            dvmh_omp_subintervals_iterator_t *it = dvmh_omp_subintervals_iterator_new(node);

            while (dvmh_omp_subintervals_iterator_has_next(it)) {
                dvmh_omp_interval_t *child = dvmh_omp_subintervals_iterator_next(it);
                dvmh_omp_runtime_context_traverse(r_ctx, child);

                // add aggregated metrics of child into same metrics of current one
                dvmh_omp_interval_add_io_time(node, dvmh_omp_interval_io_time(child));
                dvmh_omp_interval_add_barrier_time(node, dvmh_omp_interval_sync_barrier_time(child));
                dvmh_omp_interval_add_critical_time(node, dvmh_omp_interval_idle_critical_time(child));
                dvmh_omp_interval_add_flush_time(node, dvmh_omp_interval_sync_flush_time(child));

                dvmh_omp_interval_add_idle_parallel_time(node, dvmh_omp_interval_idle_parallel_time(child));
            }

            dvmh_omp_subintervals_iterator_destroy(it);
        }

        for (int tid = 0; tid < r_ctx->num_threads; ++tid) {
            dvmh_omp_thread_context_t *t_ctx = dvmh_omp_runtime_context_get_thread_context(r_ctx, tid);
            dvmh_omp_interval_t *local = dvmh_omp_thread_context_get_interval(t_ctx, id);

            if (dvmh_omp_interval_used_time(local) == 0.0)
                continue;

            // collect from thread local data
            dvmh_omp_interval_add_io_time(node, dvmh_omp_interval_io_time(local));
            dvmh_omp_interval_add_barrier_time(node, dvmh_omp_interval_sync_barrier_time(local));
            dvmh_omp_interval_add_critical_time(node, dvmh_omp_interval_idle_critical_time(local));
            dvmh_omp_interval_add_flush_time(node, dvmh_omp_interval_sync_flush_time(local));

            dvmh_omp_interval_add_used_time(node, dvmh_omp_interval_used_time(local));

            dvmh_omp_interval_add_used_threads_num(node, 1);
            dvmh_omp_interval_add_exectuion_count(node, 1);
        }

        // and some global metrics
        dvmh_omp_interval_add_execution_time(node, r_ctx->execution_times[id]);
        dvmh_omp_interval_add_idle_parallel_time(node, r_ctx->idle_parallel_times[id]);
}

dvmh_omp_interval_t *
dvmh_omp_runtime_context_integrate(
        dvmh_omp_runtime_context_t *r_ctx)
{
    assert(r_ctx != NULL);

    dvmh_omp_interval_t *tree = dvmh_omp_runtime_context_build_interval_tree(r_ctx);

    dvmh_omp_runtime_context_traverse(r_ctx, tree);

    return tree;
}

void
dvmh_omp_runtime_context_integrated_free(
        dvmh_omp_runtime_context_t *r_ctx,
        dvmh_omp_interval_t *tree)
{
    assert(r_ctx != NULL);
    assert(tree != NULL);

    for (int id = 0; id < r_ctx->num_context_descriptors; ++id) {
        dvmh_omp_interval_deinit(&tree[id]);
    }

    free(tree);
}
