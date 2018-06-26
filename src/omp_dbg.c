#include <stddef.h>
#include <assert.h>
#include <omp.h>

#include "list.h"
#include "context_descriptor.h"
#include "register_context.h"
#include "dvmh_omp_interval.h"
#include "dvmh_omp_thread_context.h"
#include "omp_dbg.h"

// Каждый поток будет иметь свой контекст.
static dvmh_omp_thread_context *thread_context;
#pragma omp threadprivate(thread_context)

// Все потоки будут использовать общие дескрипторы.
static list *registered_descriptors = NULL;

// Список контекстов во всех потоках. Нужно, чтобы иметь доступ с одного места в dbg_finalize.
static list *thread_contexts = NULL;

void clear_registered_contexts(list *registered_descriptors)
{
	assert(registered_descriptors);
	list_destroy_with(registered_descriptors, (list_element_destroy_t *) unregister_context);
}

void DBG_Init(long *ThreadID)
{   
    int size = list_size(registered_descriptors);
    assert(size > 0);

    thread_contexts = list_create();
    assert(thread_contexts != NULL);

    #pragma omp parallel firstprivate(thread_contexts, size)
    {
        // Для каждого потока создаем свой контекст.
        thread_context = dvmh_omp_thread_context_create(size);
        assert(thread_context != NULL);
        #pragma omp critical (dbg_init)
        {
            list_append_tail(thread_contexts, thread_context);
        }
    }

    return;
};

// TODO
void DBG_Finalize()
{
    list *tmp = NULL;
    #pragma omp critical (dbg_finalize)
    {
        if (registered_descriptors != NULL) {
            tmp = registered_descriptors;
            registered_descriptors = NULL;
        }
    }
    if (tmp == NULL) {
        return;
    }

    // TODO integrate intervals and print out the results.


    list_destroy_with(thread_contexts, (list_element_destroy_t *) dvmh_omp_thread_context_destroy);
    clear_registered_contexts(tmp);
    return;
};

void
DBG_Get_Handle(long *StaticContextHandle, char* ContextString, long StringLength)
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

// TODO
void DBG_BeforeParallel (long *StaticContextHandle, long *ThreadID, int *NumThreadsResults, int *IfExprResult){};


// TODO
void DBG_ParallelEvent (long *StaticContextHandle, long *ThreadID){};

// TODO
void DBG_ParallelEventEnd (long *StaticContextHandle, long *ThreadID){};

// TODO
void DBG_AfterParallel (long *StaticContextHandle, long *ThreadID){};

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

void DBG_BeforeCritical (long *StaticContextHandle, long *ThreadID){};

void DBG_CriticalEvent(long *StaticContextHandle, long *ThreadID){};

void DBG_CriticalEventEnd(long *StaticContextHandle, long *ThreadID){};

void DBG_AfterCritical(long *StaticContextHandle, long *ThreadID){};

void DBG_BeforeBarrier(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_barrier_time(i, -now);
};

void DBG_AfterBarrier(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_barrier_time(i, now);
};

void DBG_FlushEvent(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_flush_time(i, -now);
};

void DBG_FlushEventEnd(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
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
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_io_time(i, -now);
};

void DBG_AfterIO(long *StaticContextHandle, long *ThreadID)
{
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_io_time(i, now);
};

void DBG_BeforeInterval (long *StaticContextHandle, long *ThreadID, long *IntervalIndex)
{
    dvmh_omp_thread_context_enter_interval(thread_context, 0); // TODO pass interval id
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, now);
    dvmh_omp_interval_add_exectuion_count(i, 1L);
};

void DBG_AfterInterval (long *StaticContextHandle, long *ThreadID, long *IntervalIndex)
{
    dvmh_omp_interval *i= dvmh_omp_thread_context_current_interval(thread_context);
    double now = omp_get_wtime();
    dvmh_omp_interval_add_used_time(i, now);
    dvmh_omp_thread_context_leave_interval(thread_context);
};
