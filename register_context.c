#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "context_string.h"
#include "context_descriptor.h"
#include "register_context.h"

static void register_array_name_context(const char *context_string, context_descriptor *cd);
static void register_barrier_context(const char *context_string, context_descriptor *cd);
static void register_common_name_context(const char *context_string, context_descriptor *cd);
static void register_critical_context(const char *context_string, context_descriptor *cd);
static void register_filename_context(const char *context_string, context_descriptor *cd);
static void register_flush_context(const char *context_string, context_descriptor *cd);
static void register_function_context(const char *context_string, context_descriptor *cd);
static void register_func_call_context(const char *context_string, context_descriptor *cd);
static void register_interval_context(const char *context_string, context_descriptor *cd);
static void register_io_context(const char *context_string, context_descriptor *cd);
static void register_master_context(const char *context_string, context_descriptor *cd);
static void register_omploop_context(const char *context_string, context_descriptor *cd);
static void register_ordered_context(const char *context_string, context_descriptor *cd);
static void register_parallel_context(const char *context_string, context_descriptor *cd);
static void register_sections_context(const char *context_string, context_descriptor *cd);
static void register_section_event_context(const char *context_string, context_descriptor *cd);
static void register_seqloop_context(const char *context_string, context_descriptor *cd);
static void register_single_context(const char *context_string, context_descriptor *cd);
static void register_threadprivate_context(const char *context_string, context_descriptor *cd);
static void register_variable_name_context(const char *context_string, context_descriptor *cd);
static void register_workshare_context(const char *context_string, context_descriptor *cd);
static void unregister_context(context_descriptor *cd);

static list *registered_descriptors = NULL;

context_descriptor * register_context(const char *context_string)
{
	context_type type = get_context_string_type(context_string);
	assert(type != CONTEXT_UNKNOWN);

	context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);

	switch (type){
		case CONTEXT_ARRAY_NAME:
		    register_array_name_context(context_string, cd);
			break;
		case CONTEXT_BARRIER:
		    register_barrier_context(context_string, cd);
			break;
		case CONTEXT_COMMON_NAME:
			register_common_name_context(context_string, cd);
			break;
		case CONTEXT_CRITICAL:
			register_critical_context(context_string, cd);
			break;
		case CONTEXT_FILE_NAME:
			register_filename_context(context_string, cd);
			break;
		case CONTEXT_FLUSH:
			register_flush_context(context_string, cd);
			break;
		case CONTEXT_FUNCTION:
			register_function_context(context_string, cd);
			break;
		case CONTEXT_FUNC_CALL:
			register_func_call_context(context_string, cd);
			break;
		case CONTEXT_INTERVAL:
			register_interval_context(context_string, cd);
			break;
		case CONTEXT_IO:
			register_io_context(context_string, cd);
			break;
		case CONTEXT_MASTER:
			register_master_context(context_string, cd);
			break;
		case CONTEXT_OMPLOOP:
			register_omploop_context(context_string, cd);
			break;
		case CONTEXT_ORDERED:
			register_ordered_context(context_string, cd);
			break;
		case CONTEXT_PARALLEL:
			register_parallel_context(context_string, cd);
			break;
		case CONTEXT_SECTIONS:
			register_sections_context(context_string, cd);
			break;
		case CONTEXT_SECTION_EVENT:
			register_section_event_context(context_string, cd);
			break;
		case CONTEXT_SEQLOOP:
			register_seqloop_context(context_string, cd);
			break;
		case CONTEXT_SINGLE:
			register_single_context(context_string, cd);
			break;
		case CONTEXT_THREADPRIVATE:
			register_threadprivate_context(context_string, cd);
			break;
		case CONTEXT_UNKNOWN:
			break;
		case CONTEXT_VARIABLE_NAME:
			register_variable_name_context(context_string, cd);
			break;
		case CONTEXT_WORKSHARE:
			register_workshare_context(context_string, cd);
			break;
	}

	if (registered_descriptors == NULL){
		assert(registered_descriptors = list_create());
	}

	list_append_tail(registered_descriptors, cd);
	return cd;
}

static void register_array_name_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_ARRAY_NAME;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->arr_name.arr_name = get_param_str_value(context_string, "name1");
	cd->arr_name.variable_type = get_variable_rt_type(context_string);
	cd->arr_name.rank = get_param_int_value(context_string, "rank");
	cd->arr_name.is_indata = get_param_int_value(context_string, "isindata");
	cd->arr_name.is_incommon = get_param_int_value(context_string, "isincommon");
	cd->arr_name.is_insave = get_param_int_value(context_string, "isinsave");
}

static void unregister_array_name_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->arr_name.arr_name){
		free(cd->arr_name.arr_name);
	}
	free(cd);
}

static void register_barrier_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_BARRIER;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
}

static void unregister_barrier_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_common_name_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_COMMON_NAME;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->common.block_name = get_param_str_value(context_string, "name1");
	cd->common.names_components = get_param_names_list(context_string, "name2");
}

static void unregister_common_name_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->common.block_name){
		free(cd->common.block_name);
	}
	if (cd->common.names_components){
		list_destroy_with(cd->common.names_components, free);
	}
	free(cd);
}

static void register_critical_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_CRITICAL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
    cd->critical.critical_name = get_param_str_value(context_string, "name1");
}

static void unregister_critical_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->critical.critical_name){
		free(cd->critical.critical_name);
	}
	free(cd);
}

static void register_filename_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_FILE_NAME;
    cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
}

static void unregister_filename_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_flush_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_FLUSH;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->flush.names_flushed = get_param_names_list(context_string, "name1");
}

static void unregister_flush_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->flush.names_flushed){
		list_destroy_with(cd->flush.names_flushed, free);
	}
	free(cd);
}

static void register_function_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_FUNCTION;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->function.func_name = get_param_str_value(context_string, "name1");
	cd->function.args_count = get_param_int_value(context_string, "rank");
}

static void unregister_function_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->function.func_name){
		free(cd->function.func_name);
	}
	free(cd);
}

static void register_func_call_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_FUNC_CALL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->func_call.func_name = get_param_str_value(context_string, "name1");
	cd->func_call.args_count = get_param_int_value(context_string, "rank");
}

static void unregister_func_call_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->func_call.func_name){
		free(cd->func_call.func_name);
	}
	free(cd);
}

static void register_interval_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_INTERVAL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
}

static void unregister_interval_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_io_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_IO;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
}

static void unregister_io_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_master_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_MASTER;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
}

static void unregister_master_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_omploop_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_OMPLOOP;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->omploop.is_ordered = get_param_int_value(context_string, "ordered");
	cd->omploop.is_nowait = get_param_int_value(context_string, "nowait");
	cd->omploop.names_private = get_param_names_list(context_string, "private");
	cd->omploop.names_firstprivate = get_param_names_list(context_string, "firstprivate");
	cd->omploop.names_lastprivate = get_param_names_list(context_string, "lastprivate");
	cd->omploop.names_reduction = get_param_names_list(context_string, "reduction");
	cd->omploop.redop = get_redop_type(context_string);
	cd->omploop.schedule = get_schedule_type(context_string);
	cd->omploop.chunk_size = get_param_str_value(context_string, "chunk_size");
}

static void unregister_omploop_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->omploop.chunk_size){
		free(cd->omploop.chunk_size);
	}
	if (cd->omploop.names_private){
		list_destroy_with(cd->omploop.names_private, free);
	}
	if (cd->omploop.names_firstprivate){
		list_destroy_with(cd->omploop.names_firstprivate, free);
	}
	if (cd->omploop.names_lastprivate){
		list_destroy_with(cd->omploop.names_lastprivate, free);
	}
	if (cd->omploop.names_reduction){
		list_destroy_with(cd->omploop.names_reduction, free);
	}
	free(cd);
}

static void register_ordered_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_ORDERED;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
}

static void unregister_ordered_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_parallel_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_PARALLEL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->parallel.names_private = get_param_names_list(context_string, "private");
	cd->parallel.names_shared = get_param_names_list(context_string, "shared");
	cd->parallel.names_firstprivate = get_param_names_list(context_string, "firstprivate");
	cd->parallel.names_copyin = get_param_names_list(context_string, "copyin");
	cd->parallel.names_reduction = get_param_names_list(context_string, "reduction");
	cd->parallel.redop = get_redop_type(context_string);
	cd->parallel.default_behavior = get_behavior_type(context_string);
	cd->parallel.if_text = get_param_str_value(context_string, "if");
	cd->parallel.num_threads = get_param_str_value(context_string, "num_threads");
}

static void unregister_parallel_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->parallel.if_text){
		free(cd->parallel.if_text);
	}
	if (cd->parallel.num_threads){
		free(cd->parallel.num_threads);
	}
	if (cd->parallel.names_private){
		list_destroy_with(cd->parallel.names_private, free);
	}
	if (cd->parallel.names_shared){
		list_destroy_with(cd->parallel.names_shared, free);
	}
	if (cd->parallel.names_firstprivate){
		list_destroy_with(cd->parallel.names_firstprivate, free);
	}
	if (cd->parallel.names_copyin){
		list_destroy_with(cd->parallel.names_copyin, free);
	}
	if (cd->parallel.names_reduction){
		list_destroy_with(cd->parallel.names_reduction, free);
	}
	free(cd);
}

static void register_sections_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_SECTIONS;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->sections.is_nowait = get_param_int_value(context_string, "nowait");
	cd->sections.names_private = get_param_names_list(context_string, "private");
	cd->sections.names_firstprivate = get_param_names_list(context_string, "firstprivate");
	cd->sections.names_lastprivate = get_param_names_list(context_string, "lastprivate");
	cd->sections.names_reduction = get_param_names_list(context_string, "reduction");
	cd->sections.redop = get_redop_type(context_string);
}

static void unregister_sections_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->sections.names_private){
		list_destroy_with(cd->sections.names_private, free);
	}
	if (cd->sections.names_firstprivate){
		list_destroy_with(cd->sections.names_firstprivate, free);
	}
	if (cd->sections.names_lastprivate){
		list_destroy_with(cd->sections.names_lastprivate, free);
	}
	if (cd->sections.names_reduction){
		list_destroy_with(cd->sections.names_reduction, free);
	}
	free(cd);
}

static void register_section_event_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_SECTION_EVENT;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
}

static void unregister_section_event_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_seqloop_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_SEQLOOP;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
}

static void unregister_seqloop_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

static void register_single_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_SINGLE;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->single.is_nowait = get_param_int_value(context_string, "nowait");
	cd->single.names_private = get_param_names_list(context_string, "private");
	cd->single.names_firstprivate = get_param_names_list(context_string, "firstprivate");
	cd->single.names_copyprivate = get_param_names_list(context_string, "copyprivate");
}

static void unregister_single_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->single.names_private){
		list_destroy_with(cd->single.names_private, free);
	}
	if (cd->single.names_firstprivate){
		list_destroy_with(cd->single.names_firstprivate, free);
	}
	if (cd->single.names_copyprivate){
		list_destroy_with(cd->single.names_copyprivate, free);
	}
	free(cd);
}

static void register_threadprivate_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_THREADPRIVATE;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->threadprivate.names_threadprivate = get_param_names_list(context_string, "name1");
}

static void unregister_threadprivate_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->threadprivate.names_threadprivate){
		list_destroy_with(cd->threadprivate.names_threadprivate, free);
	}
	free(cd);
}

static void register_variable_name_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_VARIABLE_NAME;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->var_name.var_name = get_param_str_value(context_string, "name1");
	cd->var_name.is_indata = get_param_int_value(context_string, "isindata");
	cd->var_name.is_incommon = get_param_int_value(context_string, "isincommon");
	cd->var_name.is_insave = get_param_int_value(context_string, "isinsave");
	cd->var_name.variable_type = get_variable_rt_type(context_string);
}

static void unregister_variable_name_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	if (cd->var_name.var_name){
		free(cd->var_name.var_name);
	}
	free(cd);
}

static void register_workshare_context(const char *context_string, context_descriptor *cd)
{
    cd->info.type = CONTEXT_WORKSHARE;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->workshare.is_nowait = get_param_int_value(context_string, "nowait");
}

static void unregister_workshare_context(context_descriptor *cd)
{
	if (cd->info.file_name){
		free(cd->info.file_name);
	}
	free(cd);
}

void clear_registered_contexts()
{
	assert(registered_descriptors);
	list_destroy_with(registered_descriptors, (void (*)(void *)) unregister_context);
}

static void unregister_context(context_descriptor *cd)
{
	assert(cd);

	switch(cd->info.type){
		case CONTEXT_ARRAY_NAME:
		    unregister_array_name_context(cd);
			break;
		case CONTEXT_BARRIER:
		    unregister_barrier_context(cd);
			break;
		case CONTEXT_COMMON_NAME:
			unregister_common_name_context(cd);
			break;
		case CONTEXT_CRITICAL:
			unregister_critical_context(cd);
			break;
		case CONTEXT_FILE_NAME:
			unregister_filename_context(cd);
			break;
		case CONTEXT_FLUSH:
			unregister_flush_context(cd);
			break;
		case CONTEXT_FUNCTION:
			unregister_function_context(cd);
			break;
		case CONTEXT_FUNC_CALL:
			unregister_func_call_context(cd);
			break;
		case CONTEXT_INTERVAL:
			unregister_interval_context(cd);
			break;
		case CONTEXT_IO:
			unregister_io_context(cd);
			break;
		case CONTEXT_MASTER:
			unregister_master_context(cd);
			break;
		case CONTEXT_OMPLOOP:
			unregister_omploop_context(cd);
			break;
		case CONTEXT_ORDERED:
			unregister_ordered_context(cd);
			break;
		case CONTEXT_PARALLEL:
			unregister_parallel_context(cd);
			break;
		case CONTEXT_SECTIONS:
			unregister_sections_context(cd);
			break;
		case CONTEXT_SECTION_EVENT:
			unregister_section_event_context(cd);
			break;
		case CONTEXT_SEQLOOP:
			unregister_seqloop_context(cd);
			break;
		case CONTEXT_SINGLE:
			unregister_single_context(cd);
			break;
		case CONTEXT_THREADPRIVATE:
			unregister_threadprivate_context(cd);
			break;
		case CONTEXT_UNKNOWN:
			break;
		case CONTEXT_VARIABLE_NAME:
			unregister_variable_name_context(cd);
			break;
		case CONTEXT_WORKSHARE:
			unregister_workshare_context(cd);
			break;
	}
}
