#include <stdio.h>
#include "context_types.h"
#include "register_context.h"
#include "dvmh_omp_event.h"
#include "dvmh_omp_thread_info.h"
#include "dvmh_omp_event_analyzer.h"
#include "omp_dbg.h"

// Тут определены вспомогательные макросы для удобства
#define TO_THREAD_INFO(long_value) ((dvmh_omp_thread_info *) (long_value))
#define TO_LONG(ptr) ((long) (ptr))
#define IS_INITIALIZED(thread_id_ptr) (((int) (*(thread_id_ptr))) != -1 ? 1 : 0 )

dvmh_omp_event *global_omp_event;

dvmh_omp_event *get_parent_event(long *StaticContextHandle)
{
	context_descriptor *cd = (context_descriptor *) *StaticContextHandle;
	if (cd->type != CONTEXT_PARALLEL){
		fprintf(stderr, "Context Type is not parallel");
		return NULL;
	}
	parallel_context_descriptor *pcd =
			(parallel_context_descriptor *) cd->context_ptr;

	return (dvmh_omp_event *) pcd->parent_event;
}

void set_parent_event(long *StaticContextHandle, dvmh_omp_event *pe)
{
	context_descriptor *cd = (context_descriptor *) *StaticContextHandle;
	if (cd->type != CONTEXT_PARALLEL){
		fprintf(stderr, "Context Type is not parallel");
		return;
	}
	parallel_context_descriptor *pcd =
			(parallel_context_descriptor *) cd->context_ptr;

	pcd->parent_event = (void *) pe;
}

long DBG_Get_Addr(void  *VarPtr)
{
  return (long) VarPtr;
}

void DBG_Type_Control()
{
	fprintf (stderr, "DBG_Type_Control\n");
}

void DBG_Init(long *ThreadID)
{
	if (!IS_INITIALIZED(ThreadID)) {
		*ThreadID = TO_LONG(dvmh_omp_thread_info_create());
	}
	global_omp_event = dvmh_omp_event_create(DVMH_OMP_EVENT_THREAD);
	
	dvmh_omp_event_set_thread_id(global_omp_event, TO_LONG(ThreadID));
	dvmh_omp_event_set_begin_time(global_omp_event, omp_get_wtime());
	
	dvmh_omp_thread_info_event_occured(TO_THREAD_INFO(*ThreadID), global_omp_event);
	
	fprintf (stderr, "DBG_Init\n");
}

void DBG_Finalize()
{
	int is_master = 0;
	#pragma omp master
	{
		is_master = 1;
	}
	if (!is_master){
		return;
	}
	// Здесь никак не сможем сделать dvmh_omp_thread_info_destroy :(
	// может через глобальную переменную...хммм
	// для красоты еще и dvmh_omp_thread_info_event_finished,
	// это уже детали
	dvmh_omp_event_set_end_time(global_omp_event, omp_get_wtime());
	dvmh_omp_event_analyzer(global_omp_event);
	dvmh_omp_event_destroy(global_omp_event);
}

void DBG_Get_Handle(long *StaticContextHandle, char* ContextString, long StringLength)
{
	// fprintf (stderr, "DBG_Get_Handle: %s\n",ContextString);
	*StaticContextHandle = register_context(ContextString);
}

void DBG_BeforeParallel (long *StaticContextHandle, long *ThreadID, int *NumThreadsResults, int *IfExprResult)
{
	//fprintf(stderr, "Before parallel thread id = %ld\n", ThreadID);
	dvmh_omp_event *parent_event = dvmh_omp_thread_info_active_event(TO_THREAD_INFO(*ThreadID));
	dvmh_omp_event *event = dvmh_omp_event_create(DVMH_OMP_EVENT_PARALLEL_REGION);
	dvmh_omp_event_add_subevent(parent_event, event);
	
	dvmh_omp_event_set_thread_id(event, TO_LONG(ThreadID));
	dvmh_omp_event_set_begin_time(event, omp_get_wtime());
	
	dvmh_omp_thread_info_event_occured(TO_THREAD_INFO(*ThreadID), event);
	
	set_parent_event(StaticContextHandle, event);
 	fprintf (stderr, "DBG_BeforeParallel\n"); 
}

void DBG_ParallelEvent (long *StaticContextHandle, long *ThreadID)
{
	if (!IS_INITIALIZED(ThreadID)) {
		fprintf(stderr, "DBG_ThreadCreated\n");
		*ThreadID = TO_LONG(dvmh_omp_thread_info_create());
	}
	dvmh_omp_event *parent_event = get_parent_event(StaticContextHandle);
	dvmh_omp_event *event = dvmh_omp_event_create(DVMH_OMP_EVENT_THREAD);
	dvmh_omp_event_add_subevent(parent_event, event);
	
	dvmh_omp_event_set_thread_id(event, TO_LONG(ThreadID));
	dvmh_omp_event_set_begin_time(event, omp_get_wtime());

	dvmh_omp_thread_info_event_occured(TO_THREAD_INFO(*ThreadID), event);
 	fprintf (stderr, "DBG_ParallelEvent\n");
}

void DBG_ParallelEventEnd (long *StaticContextHandle, long *ThreadID)
{
	dvmh_omp_event *event = dvmh_omp_thread_info_active_event(TO_THREAD_INFO(*ThreadID));
	dvmh_omp_event_set_end_time(event, omp_get_wtime());
	
	dvmh_omp_thread_info_event_finished(TO_THREAD_INFO(*ThreadID));
	
	if (!dvmh_omp_thread_info_is_alive(TO_THREAD_INFO(*ThreadID))) {
		dvmh_omp_thread_info_destroy(TO_THREAD_INFO(*ThreadID));
		*ThreadID = -1; // если параллельная область крутится во внешнем цикле
	}
	fprintf (stderr, "DBG_ParallelEventEnd\n");
}

void DBG_AfterParallel (long *StaticContextHandle, long *ThreadID)
{
	dvmh_omp_event *event = dvmh_omp_thread_info_active_event(TO_THREAD_INFO(*ThreadID));
	dvmh_omp_event_set_end_time(event, omp_get_wtime());
	dvmh_omp_thread_info_event_finished(TO_THREAD_INFO(*ThreadID));
	fprintf (stderr, "DBG_AfterParallel\n");
}

void DBG_BeforeInterval (long *StaticContextHandle, long *ThreadID, long *InvervalIndex)
{
	fprintf (stderr, "DBG_BeforeInterval\n"); 
}

void DBG_AfterInterval (long *StaticContextHandle, long *ThreadID, long *IntervalIndex)
{
	fprintf (stderr, "DBG_AfterInterval\n");
}

void DBG_BeforeOMPLoop(long *StaticContextHandle, long *ThreadID, long *Init, long *Last, long *Step, int *ChunkSize)
{
 	fprintf (stderr, "DBG_BeforeOMPLoop\n");
}

void DBG_OMPIter(long *StaticContextHandle, long *ThreadID, long *Index)
{
 	fprintf (stderr, "DBG_OMPIter\n");
}

void DBG_AfterOMPLoop (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterOMPLoop\n");
}

void DBG_BeforeSections (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeSections\n");
}

void DBG_AfterSections(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterSections\n");
}

void DBG_SectionEvent(long *StaticContextHandle1, long *ThreadID)
{
 	fprintf (stderr, "DBG_SectionEvent\n");
}

void DBG_BeforeSingle (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeSingle\n");
}

void DBG_SingleEvent(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_SingleEvent\n");
}

void DBG_AfterSingle (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterSingle\n");
}

void DBG_BeforeWorkshare (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeWorkshare\n");
}

void DBG_AfterWorkshare(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterWorkshare\n");
}

void DBG_MasterBegin(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_MasterBegin\n");
}

void DBG_MasterEnd(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_MasterEnd\n");
}

void DBG_BeforeCritical (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeCritical\n");
}

void DBG_CriticalEvent(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_CriticalEvent\n");
}

void DBG_CriticalEventEnd(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_CriticalEventEnd\n");
}

void DBG_AfterCritical(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterCritical\n");
}

void DBG_BeforeBarrier(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeBarrier\n");
}

void DBG_AfterBarrier(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterBarrier\n");
}

void DBG_FlushEvent(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_FlushEvent\n");
}

void DBG_BeforeOrdered (long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeOrdered\n");
}

void DBG_OrderedEvent(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_OrderedEvent\n");
}

void DBG_AfterOrdered(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterOrdered\n");
}

void DBG_ThreadPrivateEvent(long *StaticContextHandle, long *ThreadID)
{
	//fprintf (stderr, "ThreadPrivateEvent\n");
	return;
}

void DBG_RegVar(long *StaticContextHandle, long *ThreadID, void*pAddr)
{
 	//fprintf (stderr, "DBG_RegVar\n");
 	return;
}

//TODO void DBG_UnregVar
//TODO void DBG_UnregArr
//TODO void DBG_UnregParVar
//TODO void DBG_UnregarArr

void DBG_RegArr(long *StaticContextHandle, long *ThreadID, long *ArrSize, void* pAddr)
{
 	// fprintf (stderr, "DBG_RegArr\n");
 	return;
}

void DBG_RegCommon(long *StaticContextHandle, long *ThreadID)
{
	//fprintf (stderr, "DBG_RegCommon\n");
	return;
}

void DBG_ReadVar(long* StaticContextHandle, long *ThreadID, void*pAddr, long *var_name)
{
	//fprintf (stderr, "DBG_ReadVar\n");
	return;
}

void DBG_ReadArr(long* StaticContextHandle, long *ThreadID, void*pAddr, long *var_name, void*pBase)
{
	//fprintf (stderr, "DBG_ReadArr\n");
	return;
}

void DBG_WriteVarBegin(long *StaticContextHandle, long *ThreadID, void*pAddr, long* var_name)
{
	//fprintf (stderr, "DBG_WriteVarBegin\n");
 	return;	
}

void DBG_WriteVarEnd(long *StaticContextHandle, long *ThreadID, void*pAddr, long* var_name)
{
 	//fprintf (stderr, "DBG_WriteVarEnd\n");
 	return;
}

void DBG_WriteArrBegin(long *StaticContextHandle, long *ThreadID, void*pAddr, long* var_name, void*pBase)
{
 	//fprintf (stderr, "DBG_WriteArrBegin\n");
 	return;
}

void DBG_WriteArrEnd(long *StaticContextHandle, long *ThreadID, void*pAddr, long* var_name, void*pBase)
{
 	//fprintf (stderr, "DBG_WriteArrBegin\n");
 	return;
}

void DBG_BegSL(long *StaticContextHandle, long *ThreadID, long *Init, long *Last, long *Step)
{
 	//TODO uncomment
 	//fprintf (stderr, "DBG_BegSL\n");
}

void DBG_SIter(long *StaticContextHandle, long *ThreadID, long *Index)
{
 	//TODO uncomment
 	//fprintf (stderr, "DBG_SIter\n");
}

void DBG_EndSL(long *StaticContextHandle, long *ThreadID)
{
 	//TODO uncomment
 	//fprintf (stderr, "DBG_EndSL\n");
}

void DBG_BeforeFuncCall(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeFuncCall\n");
}

void DBG_FuncParVar(long *StaticContextHandle, long *ThreadID, int *Position, void*pAddr, long *var_name, int *IsRead)
{
	//fprintf (stderr, "DBG_FuncParVar\n");
	return;
}

void DBG_FuncParArr(long *StaticContextHandle, long *ThreadID, int *Position, void*pAddr, long *var_name, void*pBase, int *IsRead)
{
	//fprintf (stderr, "DBG_FuncParArr\n");
	return;
}

void DBG_AfterFuncCall(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterFuncCall\n");
}

void DBG_FuncBegin(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_FuncBegin\n");
}

void DBG_FuncEnd(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_FuncEnd\n");
}

void DBG_RegParVar(long *StaticContextHandle, long *ThreadID, void*pAddr, int *Position)
{
	//fprintf (stderr, "DBG_RegParVar\n");
	return;
}

void DBG_RegParArr(long *StaticContextHandle, long *ThreadID, long *ArrSize, void*pAddr, int *Position)
{
	//fprintf (stderr, "DBG_RegParArr\n");
	return;
}

void DBG_SIfIter(long *StaticContextHandle, long *ThreadID, long *Index, long *IfVar)
{
 	fprintf (stderr, "DBG_SIfIter\n");
}

void DBG_OMPIfIter(long *StaticContextHandle, long *ThreadID, long *Index, long *IfVar)
{
 	fprintf (stderr, "DBG_OMPIfIter\n");
}

void DBG_BeforeIO(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_BeforeIO\n");
}

void DBG_AfterIO(long *StaticContextHandle, long *ThreadID)
{
 	fprintf (stderr, "DBG_AfterIO\n");
}
