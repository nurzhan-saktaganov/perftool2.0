#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>
#include <stdio.h>

#include "list.h"
#include "context_descriptor.h"
#include "register_context.h"
#include "dvmh_omp_interval.h"
#include "dvmh_omp_thread_context.h"
#include "dvmh_omp_runtime_context.h"
#include "omp_dbg.h"

#define ROOT_INTERVAL_CONTEXT_DESCRIPTOR "43*type=interval*file1=MAIN*line1=0*line2=0**"
#define METRICS_FILE_NAME "interval_stats_v2.csv"

// We use this threadprivate variable to direct access to own thread_id from each thread.
static int thread_id;
#pragma omp threadprivate(thread_id)

// We store runtime context here.
static dvmh_omp_runtime_context_t *runtime_context = NULL;

// Temporary list. We use this only in registration stage.
static list *registered_descriptors = NULL;

void DBG_Init(long *ThreadID)
{   
    assert(registered_descriptors != NULL);
    const int num_context_descriptors = list_size(registered_descriptors);
    assert(num_context_descriptors > 0);

    const int num_threads = omp_get_max_threads();

    runtime_context = dvmh_omp_runtime_context_create(num_threads, num_context_descriptors);

    // Move registered descriptors to runtime_context.
    {
        list_iterator *it = list_iterator_new(registered_descriptors);
        while (list_iterator_has_next(it)) {
            context_descriptor *cd = (context_descriptor *) list_iterator_next(it);
            dvmh_omp_runtime_context_set_context_descriptor(runtime_context, cd, cd->info.id);
        }
        list_iterator_destroy(it);

        list_destroy(registered_descriptors);
        registered_descriptors = NULL;
    }

    // Set thread contexts
    #pragma omp parallel firstprivate(runtime_context, num_context_descriptors)
    {
        #pragma omp critical (dbg_init)
        {
            thread_id = omp_get_thread_num();
            dvmh_omp_thread_context_t *thread_context = dvmh_omp_thread_context_create(num_context_descriptors, thread_id);
            assert(thread_context != NULL);
            dvmh_omp_runtime_context_set_thread_context(runtime_context, thread_context, thread_id);
        }
    }

    // Enter to the top level interval in master thread.
    const int root_interval_id = 0;
    dvmh_omp_thread_context_t *thread_context = dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_thread_context_enter_interval(thread_context, root_interval_id);

    // Stats
    dvmh_omp_interval_t *i = dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, -now);
    dvmh_omp_interval_add_exectuion_count(i, 1L);

    // calculate exectuion time
    dvmh_omp_runtime_context_add_exectuion_time(runtime_context, root_interval_id, -now);

    return;
};

static void
print_interval_tree_csv(
    const char *filename,
    dvmh_omp_runtime_context_t *ctx,
    dvmh_omp_interval_t *tree);

void DBG_Finalize()
{
    dvmh_omp_runtime_context_t *r_ctx;
    const int master_thread_id = 0;
    const int root_interval_id = 0;

    #pragma omp critical (dbg_finalize)
    {
        r_ctx = runtime_context;
        runtime_context = NULL;
    }

    if (r_ctx == NULL) return;

    dvmh_omp_thread_context_t *thread_context = dvmh_omp_runtime_context_get_thread_context(runtime_context, master_thread_id);
    dvmh_omp_interval_t *i = dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();

    // Stats
    dvmh_omp_interval_add_used_time(i, now);
    dvmh_omp_runtime_context_add_exectuion_time(runtime_context, root_interval_id, now);

    // Leave the top level interval in master thread
    dvmh_omp_thread_context_leave_interval(thread_context);

    dvmh_omp_interval_t *tree = dvmh_omp_runtime_context_integrate(r_ctx);

    // print out the results.
    print_interval_tree_csv(METRICS_FILE_NAME , runtime_context, tree);

    dvmh_omp_runtime_context_integrated_free(runtime_context, tree);


    for (int i = 0; i < r_ctx->num_threads; ++i) {
        dvmh_omp_thread_context_destroy(r_ctx->thread_contexts[i]);
    }

    for (int i = 0; i < r_ctx->num_context_descriptors; ++i) {
        unregister_context(r_ctx->context_descriptors[i]);
    }

    dvmh_omp_runtime_context_destroy(r_ctx);
};

void DBG_Get_Handle(long *StaticContextHandle, char* ContextString, long StringLength)
{
    // Нужно убедиться, что в типе long можем хранить адрес.
    assert(sizeof(long) == sizeof(void *));

    // Создаем список, если еще не создавали.
    if (registered_descriptors == NULL){
		assert(registered_descriptors = list_create());
        // create context descriptor for top level interval
        const int id = list_size(registered_descriptors);
        context_descriptor *cd = register_context(ROOT_INTERVAL_CONTEXT_DESCRIPTOR, id);
        list_append_tail(registered_descriptors, cd);
	}

    // Присваиваем id как порядковый номер регистрации.
    const int id = list_size(registered_descriptors);
    context_descriptor *cd = register_context(ContextString, id);

	list_append_tail(registered_descriptors, cd);

	*StaticContextHandle = (long) cd;
}

// We assume that there is no inner level parallelism.
void DBG_BeforeParallel (long *StaticContextHandle, long *ThreadID, int *NumThreadsResults, int *IfExprResult)
{
    // We are in master thread now
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);

    dvmh_omp_interval_t *i = dvmh_omp_thread_context_current_interval(thread_context);
    const int spawner_id = dvmh_omp_interval_get_id(i);
    dvmh_omp_runtime_context_set_threads_spawner_id(runtime_context, spawner_id);
};

void DBG_ParallelEvent (long *StaticContextHandle, long *ThreadID)
{
    const bool is_master_thread = thread_id == 0;
    if (is_master_thread) return;

    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);

    // Set inital 'root' interval for current thread in current parallel region
    const int spawner_id = dvmh_omp_runtime_context_get_threads_spawner_id(runtime_context);
    dvmh_omp_thread_context_enter_interval(thread_context, spawner_id);

    dvmh_omp_interval_t *i = dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, -now);
};

void DBG_ParallelEventEnd (long *StaticContextHandle, long *ThreadID)
{
    const bool is_master_thread = thread_id == 0;
    double now = omp_get_wtime();

    if (!is_master_thread) {
        dvmh_omp_thread_context_t *thread_context =
                dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
        dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
        dvmh_omp_interval_add_used_time(i, now);
        dvmh_omp_thread_context_leave_interval(thread_context);
    }

    dvmh_omp_runtime_context_end_parallel(runtime_context, thread_id, now);
};

void DBG_AfterParallel (long *StaticContextHandle, long *ThreadID)
{
    context_descriptor *cd = (context_descriptor *) StaticContextHandle;
    const int interval_id = cd->info.id;

    double now = omp_get_wtime();
    dvmh_omp_runtime_context_after_parallel(runtime_context, interval_id, now);
    dvmh_omp_runtime_context_set_threads_spawner_id(runtime_context, -1);
};

// TODO
void DBG_BeforeOMPLoop(long *StaticContextHandle, long *ThreadID, long *Init, long *Last, long *Step, int *ChunkSize){};

// TODO
void DBG_OMPIter(long *StaticContextHandle, long *ThreadID, long *Index){};

// TODO
void DBG_AfterOMPLoop (long *StaticContextHandle, long *ThreadID){};

// TODO
void DBG_BeforeSections (long *StaticContextHandle, long *ThreadID){};

// TODO
void DBG_AfterSections(long *StaticContextHandle, long *ThreadID){};

// TODO
void DBG_SectionEvent(long *StaticContextHandle1, long *ThreadID){};

// TODO
void DBG_SectionEventEnd(long *StaticContextHandle1, long *ThreadID){};

// TODO
void DBG_BeforeSingle (long *StaticContextHandle, long *ThreadID){};

// TODO
void DBG_SingleEvent(long *StaticContextHandle, long *ThreadID){};

void DBG_SingleEventEnd(long *StaticContextHandle, long *ThreadID){};

void DBG_AfterSingle (long *StaticContextHandle, long *ThreadID){};

void DBG_BeforeWorkshare (long *StaticContextHandle, long *ThreadID){};

void DBG_AfterWorkshare(long *StaticContextHandle, long *ThreadID){};

void DBG_MasterBegin(long *StaticContextHandle, long *ThreadID){};

void DBG_MasterEnd(long *StaticContextHandle, long *ThreadID){};

void DBG_BeforeCritical (long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_critical_time(i, -now);
};

void DBG_CriticalEvent(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_critical_time(i, now);
};

void DBG_CriticalEventEnd(long *StaticContextHandle, long *ThreadID){};

void DBG_AfterCritical(long *StaticContextHandle, long *ThreadID){};

void DBG_BeforeBarrier(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_barrier_time(i, -now);
};

void DBG_AfterBarrier(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_barrier_time(i, now);
};

void DBG_FlushEvent(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_flush_time(i, -now);
};

void DBG_FlushEventEnd(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_flush_time(i, now);
};

void DBG_BeforeOrdered (long *StaticContextHandle, long *ThreadID){};

void DBG_OrderedEvent(long *StaticContextHandle, long *ThreadID){};

void DBG_AfterOrdered(long *StaticContextHandle, long *ThreadID){};

void DBG_ThreadPrivateEvent(long *StaticContextHandle, long *ThreadID){};

void DBG_RegVar(long *StaticContextHandle, long *ThreadID, void*pAddr){};

void DBG_RegArr(long *StaticContextHandle, long *ThreadID, long *ArrSize, void* pAddr){};

void DBG_RegCommon(long *StaticContextHandle, long *ThreadID){};

void DBG_ReadVar(long* StaticContextHandle, long *ThreadID, void*pAddr, long *var_name){};

void DBG_ReadArr(long* StaticContextHandle, long *ThreadID, void*pAddr, long *var_name, void*pBase){};

void DBG_WriteVarBegin(long *StaticContextHandle, long *ThreadID, void*pAddr, long* var_name){};

void DBG_WriteArrBegin(long *StaticContextHandle, long *ThreadID, void*pAddr, long* var_name, void*pBase){};

void DBG_WriteEnd(long* StaticContextHandle, long *ThreadID, void*pAddr, long* var_name){};

void DBG_BegSL(long *StaticContextHandle, long *ThreadID, long *Init, long *Last, long *Step){};

void DBG_SIter(long *StaticContextHandle, long *ThreadID, long *Index){};

void DBG_EndSL(long *StaticContextHandle, long *ThreadID){};

void DBG_BeforeFuncCall(long *StaticContextHandle, long *ThreadID){};

void DBG_FuncParVar(long *StaticContextHandle, long *ThreadID, int *Position, void*pAddr, long *var_name, int *IsRead){};

void DBG_FuncParArr(long *StaticContextHandle, long *ThreadID, int *Position, void*pAddr, long *var_name, void*pBase, int *IsRead){};

void DBG_AfterFuncCall(long *StaticContextHandle, long *ThreadID){};

void DBG_FuncBegin(long *StaticContextHandle, long *ThreadID){};

void DBG_FuncEnd(long *StaticContextHandle, long *ThreadID){};

void DBG_RegParVar(long *StaticContextHandle, long *ThreadID, void*pAddr, int *Position){};

void DBG_RegParArr(long *StaticContextHandle, long *ThreadID, long *ArrSize, void*pAddr, int *Position){};

void DBG_SIfIter(long *StaticContextHandle, long *ThreadID, long *Index, long *IfVar){};

void DBG_OMPIfIter(long *StaticContextHandle, long *ThreadID, long *Index, long *IfVar){};

void DBG_BeforeIO(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_io_time(i, -now);
};

void DBG_AfterIO(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_io_time(i, now);
};

void DBG_BeforeInterval (long *StaticContextHandle, long *ThreadID, long *IntervalIndex)
{
    context_descriptor *cd = (context_descriptor *) StaticContextHandle;
    const int interval_id = cd->info.id;
    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);

    dvmh_omp_interval_t *parent = dvmh_omp_thread_context_current_interval(thread_context);
    const int parent_id = dvmh_omp_interval_get_id(parent);

    dvmh_omp_thread_context_enter_interval(thread_context, interval_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);

    dvmh_omp_interval_set_parent_id(i, parent_id);

    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, -now);
    dvmh_omp_interval_add_exectuion_count(i, 1L);

    // calculate execution time.
    dvmh_omp_runtime_context_lock_interval(runtime_context, interval_id);
    if (dvmh_omp_runtime_context_get_interval_visitors(runtime_context, interval_id) == 0) {
        dvmh_omp_runtime_context_add_exectuion_time(runtime_context, interval_id, -now);
    }
    dvmh_omp_runtime_context_inc_interval_visitors(runtime_context, interval_id);
    dvmh_omp_runtime_context_unlock_interval(runtime_context, interval_id);
};

void DBG_AfterInterval (long *StaticContextHandle, long *ThreadID, long *IntervalIndex)
{
    context_descriptor *cd = (context_descriptor *) StaticContextHandle;
    const int interval_id = cd->info.id;

    dvmh_omp_thread_context_t *thread_context =
            dvmh_omp_runtime_context_get_thread_context(runtime_context, thread_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, now);
    dvmh_omp_thread_context_leave_interval(thread_context);

    // calculate execution time.
    dvmh_omp_runtime_context_lock_interval(runtime_context, interval_id);
    dvmh_omp_runtime_context_dec_interval_visitors(runtime_context, interval_id);
    if (dvmh_omp_runtime_context_get_interval_visitors(runtime_context, interval_id) == 0) {
        dvmh_omp_runtime_context_add_exectuion_time(runtime_context, interval_id, now);
    }
    dvmh_omp_runtime_context_unlock_interval(runtime_context, interval_id);
};

static void
print_interval_node(
    FILE *fd,
    dvmh_omp_runtime_context_t *ctx,
    dvmh_omp_interval_t *node
)
{
    assert(node != NULL);

    if (dvmh_omp_interval_has_subintervals(node)) {
        dvmh_omp_subintervals_iterator_t *it = dvmh_omp_subintervals_iterator_new(node);

        while (dvmh_omp_subintervals_iterator_has_next(it)) {
            dvmh_omp_interval_t *child = dvmh_omp_subintervals_iterator_next(it);
            print_interval_node(fd, ctx, child);
        }

        dvmh_omp_subintervals_iterator_destroy(it);
    }

    const int id = dvmh_omp_interval_get_id(node);
    context_descriptor *cd = dvmh_omp_runtime_context_context_descriptor(ctx, id);

    /* write row */
    fprintf(fd, "%d,", id);
    fprintf(fd, "%d,", dvmh_omp_interval_execution_count(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_io_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_execution_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_sync_barrier_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_used_time(node));
    fprintf(fd, "%d,", dvmh_omp_interval_used_threads_num(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_idle_critical_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_sync_flush_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_idle_parallel_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_load_imbalance_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_thread_prod_max(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_thread_prod_min(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_thread_prod_avg(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_total_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_lost_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_productive_time(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_insufficient_parallelism(node));
    fprintf(fd, "%lf,", dvmh_omp_interval_efficiency(node));
    fprintf(fd, "%d,", dvmv_omp_interval_is_in_parallel(node));
    fprintf(fd, "%d,", cd->info.begin_line);
    fprintf(fd, "%s,", cd->info.file_name);

    if (dvmh_omp_interval_get_id(node) == 0) {
        fprintf(fd, "%d", dvmh_omp_interval_get_parent_id(node));
    } else {
        fprintf(fd, "%d\r\n", dvmh_omp_interval_get_parent_id(node));
    }
}

static void
print_interval_tree_csv(
    const char *filename,
    dvmh_omp_runtime_context_t *ctx,
    dvmh_omp_interval_t *tree)
{
    assert(tree != NULL);
    assert(filename != NULL);

    FILE *fd = fopen(filename, "w");
    assert(fd != NULL);

    /* write header */
    fprintf(fd, "id,");
	fprintf(fd, "calls count,");
	fprintf(fd, "io time,");
	fprintf(fd, "execution time,");
	fprintf(fd, "idle barrier time,");
	fprintf(fd, "used time,");
	fprintf(fd, "used threads,");
	fprintf(fd, "idle critical time,");
	fprintf(fd, "flush time,");
	fprintf(fd, "idle parallel,");
	fprintf(fd, "load imbalance time,");
	fprintf(fd, "thread prod max,");
	fprintf(fd, "thread prod min,");
	fprintf(fd, "thread prod avg,");
	fprintf(fd, "total time,");
	fprintf(fd, "lost time,");
	fprintf(fd, "productive time,");
	fprintf(fd, "insufficient parallelism,");
	fprintf(fd, "efficiency,");
	fprintf(fd, "in parallel,");
	fprintf(fd, "begin line,");
	fprintf(fd, "file name,");
	fprintf(fd, "parent id\r\n");
    /* write rows */
    print_interval_node(fd, ctx, tree);

    fclose(fd);
}
