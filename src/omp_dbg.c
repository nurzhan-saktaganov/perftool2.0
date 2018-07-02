#include <stddef.h>
#include <assert.h>
#include <omp.h>

#include "list.h"
#include "context_descriptor.h"
#include "register_context.h"
#include "dvmh_omp_interval.h"
#include "dvmh_omp_thread_context.h"
#include "dvmh_omp_runtime_context.h"
#include "omp_dbg.h"

// We use this threadprivate variable to direct access to own thread context from each thread.
static dvmh_omp_thread_context *thread_context;
#pragma omp threadprivate(thread_context)

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
            const int thread_id = omp_get_thread_num();
            thread_context = dvmh_omp_thread_context_create(num_context_descriptors, thread_id);
            assert(thread_context != NULL);
            dvmh_omp_runtime_context_set_thread_context(runtime_context, thread_context, thread_id);
        }
    }

    return;
};

void DBG_Finalize()
{
    dvmh_omp_runtime_context_t *r_ctx;

    #pragma omp critical (dbg_finalize)
    {
        r_ctx = runtime_context;
        runtime_context = NULL;
    }

    if (r_ctx == NULL) return;

    // TODO integrate intervals and print out the results.



    // Cleanup stage
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
	}

    // Присваиваем id как порядковый номер возникновения.
    const int id = list_size(registered_descriptors);
    context_descriptor *cd = register_context(ContextString, id);

	list_append_tail(registered_descriptors, cd);

	*StaticContextHandle = (long) cd;
}

// We assume that there is no inner level parallelism.
void DBG_BeforeParallel (long *StaticContextHandle, long *ThreadID, int *NumThreadsResults, int *IfExprResult)
{
    const int is_master_thread = 1;
    *ThreadID = is_master_thread;
};

void DBG_ParallelEvent (long *StaticContextHandle, long *ThreadID)
{
    const int is_master_thread = *ThreadID;
    if (is_master_thread) return;
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, -now);
};

void DBG_ParallelEventEnd (long *StaticContextHandle, long *ThreadID)
{
    const int is_master_thread = *ThreadID;
    double now = omp_get_wtime();

    if (!is_master_thread) {
        dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
        dvmh_omp_interval_add_used_time(i, now);
    }

    const int thread_id = dvmh_omp_thread_context_thread_id(thread_context);
    dvmh_omp_runtime_context_end_parallel(runtime_context, thread_id, now);
};

void DBG_AfterParallel (long *StaticContextHandle, long *ThreadID)
{
    context_descriptor *cd = (context_descriptor *) StaticContextHandle;
    const int interval_id = cd->info.id;

    double now = omp_get_wtime();
    dvmh_omp_runtime_context_after_parallel(runtime_context, interval_id, now);
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
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_critical_time(i, -now);
};

void DBG_CriticalEvent(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_critical_time(i, now);
};

void DBG_CriticalEventEnd(long *StaticContextHandle, long *ThreadID){};

void DBG_AfterCritical(long *StaticContextHandle, long *ThreadID){};

void DBG_BeforeBarrier(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_barrier_time(i, -now);
};

void DBG_AfterBarrier(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_barrier_time(i, now);
};

void DBG_FlushEvent(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_flush_time(i, -now);
};

void DBG_FlushEventEnd(long *StaticContextHandle, long *ThreadID)
{
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
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_io_time(i, -now);
};

void DBG_AfterIO(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_io_time(i, now);
};

void DBG_BeforeInterval (long *StaticContextHandle, long *ThreadID, long *IntervalIndex)
{
    context_descriptor *cd = (context_descriptor *) StaticContextHandle;
    const int interval_id = cd->info.id;
    dvmh_omp_thread_context_enter_interval(thread_context, interval_id);
    dvmh_omp_interval_t *i= dvmh_omp_thread_context_current_interval(thread_context);
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
