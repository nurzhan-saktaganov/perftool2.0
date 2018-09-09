#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <string.h>

#include "dvmh_omp_runtime_context.h"

void
dvmh_omp_runtime_context_init(
        dvmh_omp_runtime_context_t *ctx,
        int num_threads,
        int num_context_descriptors)
{
    assert(ctx != NULL);
    assert(num_threads > 0);
    assert(num_context_descriptors > 0);

    ctx->threads_spawner_interval_id = DVMH_OMP_INTERVAL_ID_UNDEFINED;

    // thread contexts
    ctx->num_threads = num_threads;
    ctx->thread_contexts = (dvmh_omp_thread_context_t *) malloc(num_threads * sizeof(dvmh_omp_thread_context_t));
    assert(ctx->thread_contexts != NULL);
    for (int i = 0; i < num_threads; ++i) {
        dvmh_omp_thread_context_init(&ctx->thread_contexts[i], num_context_descriptors, i);
    }

    // context descriptors
    ctx->num_context_descriptors = num_context_descriptors;
    ctx->context_descriptors = (context_descriptor **) calloc(num_context_descriptors, sizeof(context_descriptor *));
    assert(ctx->context_descriptors != NULL);

    // intervals locks
    ctx->interval_locks = (omp_lock_t *) malloc(num_context_descriptors * sizeof(omp_lock_t));
    assert(ctx->interval_locks != NULL);
    for (int i = 0; i < num_context_descriptors; ++i) {
        omp_init_lock(&ctx->interval_locks[i]);
    }

    // rest global data
    ctx->threads_in_interval = (int *) calloc(num_context_descriptors, sizeof(int));
    assert(ctx->threads_in_interval != NULL);

    ctx->execution_times = (double *) calloc(num_context_descriptors, sizeof(double));
    assert(ctx->execution_times != NULL);

    ctx->end_parallel_times = (double *) calloc(num_context_descriptors, sizeof(double));
    assert(ctx->end_parallel_times != NULL);

    ctx->idle_parallel_times = (double *) calloc(num_context_descriptors, sizeof(double));
    assert(ctx->idle_parallel_times != NULL);

    ctx->thread_prod_times = (double *) calloc(num_threads, sizeof(double));
    assert(ctx->thread_prod_times != NULL);

    ctx->is_interval_threads_spawner = (bool *) calloc(num_context_descriptors, sizeof(bool));
    assert(ctx->is_interval_threads_spawner != NULL);

    ctx->is_interval_in_parallel = (bool *) calloc(num_context_descriptors, sizeof(bool));
    assert(ctx->is_interval_in_parallel != NULL);
    // Is interval has been spawned in parallel region?
    // Set default value to true.
    // So we have to set it to false only for non-parallel intervals.
    // Therefore there is no race-condition.
    for (int i = 0; i < num_context_descriptors; ++i) {
        ctx->is_interval_in_parallel[i] = true;
    }

    ctx->is_parallel_region = false;
}

void
dvmh_omp_runtime_context_deinit(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    assert(ctx->num_threads > 0);
    assert(ctx->num_context_descriptors > 0);

    // thread contexts
    assert(ctx->thread_contexts != NULL);
    for (int i = 0; i < ctx->num_threads; ++i) {
        dvmh_omp_thread_context_deinit(&ctx->thread_contexts[i]);
    }
    free(ctx->thread_contexts);
    ctx->thread_contexts = NULL;

    // context descriptors
    assert(ctx->context_descriptors != NULL);
    free(ctx->context_descriptors);
    ctx->context_descriptors = NULL;

    // intervals locks
    assert(ctx->interval_locks != NULL);
    for (int i = 0; i < ctx->num_context_descriptors; ++i) {
        omp_destroy_lock(&ctx->interval_locks[i]);
    }
    free(ctx->interval_locks);
    ctx->interval_locks = NULL;

    // rest global data
    assert(ctx->threads_in_interval != NULL);
    free(ctx->threads_in_interval);
    ctx->threads_in_interval = NULL;

    assert(ctx->execution_times != NULL);
    free(ctx->execution_times);
    ctx->execution_times = NULL;

    assert(ctx->end_parallel_times != NULL);
    free(ctx->end_parallel_times);
    ctx->end_parallel_times = NULL;

    assert(ctx->idle_parallel_times != NULL);
    free(ctx->idle_parallel_times);
    ctx->idle_parallel_times = NULL;

    assert(ctx->thread_prod_times != NULL);
    free(ctx->thread_prod_times);
    ctx->thread_prod_times = NULL;

    assert(ctx->is_interval_threads_spawner != NULL);
    free(ctx->is_interval_threads_spawner);
    ctx->is_interval_threads_spawner = NULL;

    assert(ctx->is_interval_in_parallel != NULL);
    free(ctx->is_interval_in_parallel);
    ctx->is_interval_in_parallel = NULL;
}

dvmh_omp_thread_context_t *
dvmh_omp_runtime_context_get_thread_context(
        dvmh_omp_runtime_context_t *ctx,
        int thread_id)
{
    assert(ctx != NULL);
    assert(ctx->thread_contexts != NULL);
    assert(0 <= thread_id && thread_id < ctx->num_threads);
    return &ctx->thread_contexts[thread_id];
}

void
dvmh_omp_runtime_context_set_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        context_descriptor *cd,
        int cd_id)
{
    assert(ctx != NULL);
    assert(cd != NULL);
    assert(ctx->context_descriptors != NULL);
    assert(0 <= cd_id && cd_id < ctx->num_context_descriptors);
    ctx->context_descriptors[cd_id] = cd;
}

context_descriptor *
dvmh_omp_runtime_context_context_descriptor(
        dvmh_omp_runtime_context_t *ctx,
        int cd_id)
{
    assert(ctx != NULL);
    assert(ctx->context_descriptors != NULL);
    assert(0 <= cd_id && cd_id < ctx->num_context_descriptors);
    return ctx->context_descriptors[cd_id];
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

void
dvmh_omp_runtime_context_unset_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    ctx->threads_spawner_interval_id = DVMH_OMP_INTERVAL_ID_UNDEFINED;
}

int
dvmh_omp_runtime_context_get_threads_spawner_id(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    return ctx->threads_spawner_interval_id;
}

void
dvmh_omp_runtime_context_set_parallel_mode(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    ctx->is_parallel_region = true;
}

void
dvmh_omp_runtime_context_unset_parallel_mode(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    ctx->is_parallel_region = false;
}

bool
dvmh_omp_runtime_context_is_parallel_mode(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);
    return ctx->is_parallel_region;
}

void
dvmh_omp_runtime_context_set_interval_non_parallel(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(ctx->is_interval_in_parallel != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->is_interval_in_parallel[id] = false;
}

void
dvmh_omp_runtime_context_set_interval_threads_spawner(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(ctx->is_interval_threads_spawner != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->is_interval_threads_spawner[id] = true;
}

void
dvmh_omp_runtime_context_lock_interval(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(ctx->interval_locks != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    omp_set_lock(&ctx->interval_locks[id]);
}

void
dvmh_omp_runtime_context_unlock_interval(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(ctx->interval_locks != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    omp_unset_lock(&ctx->interval_locks[id]);
}


// must be under lock.
void
dvmh_omp_runtime_context_inc_interval_visitors(
        dvmh_omp_runtime_context_t *ctx,
        int id)
{
    assert(ctx != NULL);
    assert(ctx->threads_in_interval != NULL);
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
    assert(ctx->threads_in_interval != NULL);
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
    assert(ctx->threads_in_interval != NULL);
    assert(0 <= id && id < ctx->num_context_descriptors);
    ctx->threads_in_interval[id]--;
}

// must be under lock.
void
dvmh_omp_runtime_context_add_execution_time(
        dvmh_omp_runtime_context_t *ctx,
        int id,
        double execution_time)
{
    assert(ctx != NULL);
    assert(ctx->execution_times != NULL);
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
    assert(ctx->end_parallel_times != NULL);
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
    assert(ctx->end_parallel_times != NULL);
    assert(ctx->idle_parallel_times != NULL);
    assert(0 <= interval_id && interval_id < ctx->num_context_descriptors);
    // TODO maybe subtract from max end_parallel_times? See perftool sources.
    for (int i = 0; i < ctx->num_threads; ++i) {
        ctx->idle_parallel_times[interval_id] += when - ctx->end_parallel_times[i];
    }
}

// just creates links between intervals
static void
dvmh_omp_runtime_context_build_interval_tree(
        dvmh_omp_runtime_context_t *ctx,
        dvmh_omp_interval_t *tree)
{
    assert(ctx != NULL);
    assert(tree != NULL);

    // number of intervals is no more than context descriptors.
    // element at position i indicates a parent id of interval with id=i.
    int *parents = (int *) malloc(ctx->num_context_descriptors * sizeof(int));
    assert(parents != NULL);

    for (int i = 0; i < ctx->num_context_descriptors; ++i ) {
        parents[i] = DVMH_OMP_INTERVAL_ID_UNDEFINED;
    }

    // retrieve parent id of each interval
    for (int thread_id = 0; thread_id < ctx->num_threads; ++thread_id) {
        dvmh_omp_thread_context_t *tctx = dvmh_omp_runtime_context_get_thread_context(ctx, thread_id);
        for (int id = 0; id < ctx->num_context_descriptors; ++id) {
            dvmh_omp_interval_t *i = dvmh_omp_thread_context_get_interval(tctx, id);

            if (!dvmh_omp_interval_has_been_executed(i)) {
                continue;
            }

            const int parent_id = dvmh_omp_interval_get_parent_id(i);

            // if interval has been executed, it has parent id
            // or it's parent id is DVMH_OMP_INTERVAL_NO_PARENT (for root and thread local root intervals)
            assert(parent_id != DVMH_OMP_INTERVAL_ID_UNDEFINED);

            // Skip thread local root intervals. Their parent ids conflict
            // with parent id of same interval in master thread.
            if (parent_id == DVMH_OMP_INTERVAL_NO_PARENT && id != 0) {
                continue;
            }

            // if parent has not been set yet.
            if (parents[id] == DVMH_OMP_INTERVAL_ID_UNDEFINED) {
                parents[id] = parent_id;
            }

            // check interval has same parent in different threads.
            assert(parents[id] == parent_id);
        }
    }

    // make connections
    for (int id = 0; id < ctx->num_context_descriptors; ++id) {
        const int parent_id = parents[id];

        // interval has not been executed in any thread
        if (parent_id == DVMH_OMP_INTERVAL_ID_UNDEFINED) {
            continue;
        }

        dvmh_omp_interval_t *i = &tree[id];

        dvmh_omp_interval_set_parent_id(i, parent_id);

        // root interval
        if (parent_id == DVMH_OMP_INTERVAL_NO_PARENT) {
            continue;
        }

        dvmh_omp_interval_t *p = &tree[parent_id];

        // make connection
        dvmh_omp_interval_add_subinterval(p, i);
    }

    free(parents);
}

static void
dvmh_omp_runtime_context_collect_metrics(
        dvmh_omp_runtime_context_t *ctx,
        dvmh_omp_interval_t *node)
{
        assert(ctx != NULL);
        assert(node != NULL);

        const int node_id = dvmh_omp_interval_get_id(node);

        double *interval_thread_prod_times = (double *) malloc(ctx->num_threads * sizeof(double));
        assert(interval_thread_prod_times != NULL);
        memcpy(interval_thread_prod_times, ctx->thread_prod_times, ctx->num_threads * sizeof(double));

        // breadth first search approach
        int max_used_threads_num = 1;
        if (dvmh_omp_interval_has_subintervals(node)) {
            dvmh_omp_subintervals_iterator_t *it = dvmh_omp_subintervals_iterator_new(node);
            while (dvmh_omp_subintervals_iterator_has_next(it)) {
                dvmh_omp_interval_t *child = dvmh_omp_subintervals_iterator_next(it);
                dvmh_omp_runtime_context_collect_metrics(ctx, child);

                // add aggregated metrics of child into same metrics of current one
                dvmh_omp_interval_add_io_time(node, dvmh_omp_interval_io_time(child));
                dvmh_omp_interval_add_barrier_time(node, dvmh_omp_interval_sync_barrier_time(child));
                dvmh_omp_interval_add_critical_time(node, dvmh_omp_interval_idle_critical_time(child));
                dvmh_omp_interval_add_flush_time(node, dvmh_omp_interval_sync_flush_time(child));

                dvmh_omp_interval_add_idle_parallel_time(node, dvmh_omp_interval_idle_parallel_time(child));

                dvmh_omp_interval_add_extra_used_time(node, dvmh_omp_interval_extra_used_time(child));
                dvmh_omp_interval_add_used_time(node, dvmh_omp_interval_extra_used_time(child));

                const int used_threads_number = dvmh_omp_interval_used_threads_num(child);
                if (used_threads_number > max_used_threads_num) {
                    max_used_threads_num = used_threads_number;
                }
            }
            dvmh_omp_subintervals_iterator_destroy(it);
        }

        if (ctx->is_interval_in_parallel[node_id]) {
            dvmh_omp_interval_set_parallel(node);
        }

        const bool is_threads_spawner = ctx->is_interval_threads_spawner[node_id];

        for (int thread_id = 0; thread_id < ctx->num_threads; ++thread_id) {
            if (is_threads_spawner || dvmh_omp_interval_is_in_parallel(node)) {
                dvmh_omp_thread_context_t *tctx = dvmh_omp_runtime_context_get_thread_context(ctx, thread_id);
                dvmh_omp_interval_t *local = dvmh_omp_thread_context_get_interval(tctx, node_id);
                interval_thread_prod_times[thread_id] = dvmh_omp_interval_productive_time(local);
                if (is_threads_spawner) {
                    ctx->thread_prod_times[thread_id] += dvmh_omp_interval_productive_time(local);
                }
            } else {
                interval_thread_prod_times[thread_id] = ctx->thread_prod_times[thread_id] - interval_thread_prod_times[thread_id];
            }
        }

        if (!is_threads_spawner && !dvmh_omp_interval_is_in_parallel(node)) {
            const int master_thread_id = 0;
            dvmh_omp_thread_context_t *tctx = dvmh_omp_runtime_context_get_thread_context(ctx, master_thread_id);
            dvmh_omp_interval_t *local = dvmh_omp_thread_context_get_interval(tctx, node_id);
            interval_thread_prod_times[master_thread_id] = dvmh_omp_interval_productive_time(local);
        }

        double thread_prod_avg = 0.0;
        double thread_prod_max = 0.0;
        double thread_prod_min = DBL_MAX;
        int thread_prod_avg_divider = 0;

        for (int thread_id = 0; thread_id < ctx->num_threads; ++thread_id) {

#ifdef PERFTOOL_1_COMPATIBILIY
            if (interval_thread_prod_times[thread_id] == 0.0) {
                continue;
            }
#endif

            thread_prod_avg_divider++;

            const double thread_prod_time = interval_thread_prod_times[thread_id];
            if (thread_prod_time < thread_prod_min) thread_prod_min = thread_prod_time;
            if (thread_prod_time > thread_prod_max) thread_prod_max = thread_prod_time;
            thread_prod_avg += thread_prod_time;
        }

        thread_prod_avg /= thread_prod_avg_divider;

        dvmh_omp_interval_set_thread_prod_avg(node, thread_prod_avg);
        dvmh_omp_interval_set_thread_prod_max(node, thread_prod_max);
        dvmh_omp_interval_set_thread_prod_min(node, thread_prod_min);

        // collect metrics from thread local data
        for (int thread_id = 0; thread_id < ctx->num_threads; ++thread_id) {
            dvmh_omp_thread_context_t *tctx = dvmh_omp_runtime_context_get_thread_context(ctx, thread_id);
            dvmh_omp_interval_t *local = dvmh_omp_thread_context_get_interval(tctx, node_id);

#ifdef PERFTOOL_1_COMPATIBILIY
            if (interval_thread_prod_times[thread_id] == 0.0 ) {
                continue;
            }
#endif

            dvmh_omp_interval_add_load_imbalance(node, thread_prod_max - interval_thread_prod_times[thread_id]);

            dvmh_omp_interval_add_io_time(node, dvmh_omp_interval_io_time(local));
            dvmh_omp_interval_add_barrier_time(node, dvmh_omp_interval_sync_barrier_time(local));
            dvmh_omp_interval_add_critical_time(node, dvmh_omp_interval_idle_critical_time(local));
            dvmh_omp_interval_add_flush_time(node, dvmh_omp_interval_sync_flush_time(local));

            dvmh_omp_interval_add_used_time(node, dvmh_omp_interval_used_time(local));

            dvmh_omp_interval_add_execution_count(node, dvmh_omp_interval_execution_count(local));

            if (is_threads_spawner && thread_id != 0) {
                dvmh_omp_interval_add_extra_used_time(node, dvmh_omp_interval_used_time(local));
            }
        }

        // and some global metrics
        dvmh_omp_interval_add_execution_time(node, ctx->execution_times[node_id]);
        dvmh_omp_interval_add_idle_parallel_time(node, ctx->idle_parallel_times[node_id]);

        if (is_threads_spawner || dvmh_omp_interval_is_in_parallel(node)) {
            dvmh_omp_interval_set_used_threads_num(node, ctx->num_threads);
        } else {
            dvmh_omp_interval_set_used_threads_num(node, max_used_threads_num);
        }

        free(interval_thread_prod_times);
}

dvmh_omp_interval_t *
dvmh_omp_runtime_context_integrate(
        dvmh_omp_runtime_context_t *ctx)
{
    assert(ctx != NULL);

    dvmh_omp_interval_t *tree = (dvmh_omp_interval_t *) malloc(ctx->num_context_descriptors * sizeof(dvmh_omp_interval_t));
    assert(tree != NULL);

    for (int id = 0; id < ctx->num_context_descriptors; ++id) {
        dvmh_omp_interval_init(&tree[id]);
        dvmh_omp_interval_set_id(&tree[id], id);
    }

    // create links
    dvmh_omp_runtime_context_build_interval_tree(ctx, tree);

    // It will be modified during metrics collection.
    // We need to zeroing it in case of multiple calls of current function.
    memset(ctx->thread_prod_times, 0, ctx->num_threads * sizeof(double));

    // collect metrics
    dvmh_omp_interval_t *root = tree;

    dvmh_omp_runtime_context_collect_metrics(ctx, root);

    return tree;
}

void
dvmh_omp_runtime_context_integrated_free(
        dvmh_omp_runtime_context_t *ctx,
        dvmh_omp_interval_t *tree)
{
    assert(ctx != NULL);
    assert(tree != NULL);

    for (int id = 0; id < ctx->num_context_descriptors; ++id) {
        dvmh_omp_interval_deinit(&tree[id]);
    }

    free(tree);
}
