#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "context_string.h"
#include "context_descriptor.h"
#include "register_context.h"

static context_descriptor *register_array_name_context(const char *context_string);
static context_descriptor *register_barrier_context(const char *context_string);
static context_descriptor *register_common_name_context(const char *context_string);
static context_descriptor *register_critical_context(const char *context_string);
static context_descriptor *register_filename_context(const char *context_string);
static context_descriptor *register_flush_context(const char *context_string);
static context_descriptor *register_function_context(const char *context_string);
static context_descriptor *register_func_call_context(const char *context_string);
static context_descriptor *register_interval_context(const char *context_string);
static context_descriptor *register_master_context(const char *context_string);
static context_descriptor *register_omploop_context(const char *context_string);
static context_descriptor *register_ordered_context(const char *context_string);
static context_descriptor *register_parallel_context(const char *context_string);
static context_descriptor *register_sections_context(const char *context_string);
static context_descriptor *register_section_event_context(const char *context_string);
static context_descriptor *register_seqloop_context(const char *context_string);
static context_descriptor *register_single_context(const char *context_string);
static context_descriptor *register_threadprivate_context(const char *context_string);
static context_descriptor *register_variable_name_context(const char *context_string);
static context_descriptor *register_workshare_context(const char *context_string);
static void unregister_context(context_descriptor *cd);

static list *registered_descriptors = NULL;

context_descriptor * register_context(const char *context_string)
{
	context_descriptor *cd;

	context_type type = get_context_string_type(context_string);
	switch (type){
		case CONTEXT_ARRAY_NAME:
		    cd = register_array_name_context(context_string);
			break;
		case CONTEXT_BARRIER:
		    cd = register_barrier_context(context_string);
			break;
		case CONTEXT_COMMON_NAME:
			cd = register_common_name_context(context_string);
			break;
		case CONTEXT_CRITICAL:
			cd = register_critical_context(context_string);
			break;
		case CONTEXT_FILE_NAME:
			cd = register_filename_context(context_string);
			break;
		case CONTEXT_FLUSH:
			cd = register_flush_context(context_string);
			break;
		case CONTEXT_FUNCTION:
			cd = register_function_context(context_string);
			break;
		case CONTEXT_FUNC_CALL:
			cd = register_func_call_context(context_string);
			break;
		case CONTEXT_INTERVAL:
			cd = register_interval_context(context_string);
			break;
		case CONTEXT_MASTER:
			cd = register_master_context(context_string);
			break;
		case CONTEXT_OMPLOOP:
			cd = register_omploop_context(context_string);
			break;
		case CONTEXT_ORDERED:
			cd = register_ordered_context(context_string);
			break;
		case CONTEXT_PARALLEL:
			cd = register_parallel_context(context_string);
			break;
		case CONTEXT_SECTIONS:
			cd = register_sections_context(context_string);
			break;
		case CONTEXT_SECTION_EVENT:
			cd = register_section_event_context(context_string);
			break;
		case CONTEXT_SEQLOOP:
			cd = register_seqloop_context(context_string);
			break;
		case CONTEXT_SINGLE:
			cd = register_single_context(context_string);
			break;
		case CONTEXT_THREADPRIVATE:
			cd = register_threadprivate_context(context_string);
			break;
		case CONTEXT_UNKNOWN:
			cd = NULL;
			break;
		case CONTEXT_VARIABLE_NAME:
			cd = register_variable_name_context(context_string);
			break;
		case CONTEXT_WORKSHARE:
			cd = register_workshare_context(context_string);
			break;
		default:
			cd = NULL;
            break;
	}

	if (registered_descriptors == NULL){
		registered_descriptors = list_create();
		assert(registered_descriptors);
	}

	if (cd == NULL){
		fprintf(stderr, "Unknown context string format: %s\n", context_string);	
	} else {
		list_append_tail(registered_descriptors, cd);
	}
	return cd;
}

static context_descriptor *register_array_name_context(const char *context_string)
{
	context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
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
	return cd;
}

static void unregister_array_name_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->arr_name.arr_name != NULL){
		free(cd->arr_name.arr_name);
	}
	free(cd);
}

static context_descriptor *register_barrier_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_BARRIER;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	return cd;
}

static void unregister_barrier_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_common_name_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_COMMON_NAME;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->common.block_name = get_param_str_value(context_string, "name1");
	cd->common.names_components = get_param_names_list(context_string, "name2");
	return cd;
}

static void unregister_common_name_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->common.block_name != NULL){
		free(cd->common.block_name);
	}
	if (cd->common.names_components != NULL){
		list_destroy(cd->common.names_components, free);
	}
	free(cd);
}

static context_descriptor *register_critical_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_CRITICAL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
    cd->critical.critical_name = get_param_str_value(context_string, "name1");
	return cd;
}

static void unregister_critical_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->critical.critical_name != NULL){
		free(cd->critical.critical_name);
	}
	free(cd);
}

static context_descriptor *register_filename_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_FILE_NAME;
    cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	return cd;
}

static void unregister_filename_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_flush_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_FLUSH;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->flush.names_flushed = get_param_names_list(context_string, "name1");
	return cd;
}

static void unregister_flush_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->flush.names_flushed != NULL){
		list_destroy(cd->flush.names_flushed, free);
	}
	free(cd);
}

static context_descriptor *register_function_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_FUNCTION;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->function.func_name = get_param_str_value(context_string, "name1");
	cd->function.args_count = get_param_int_value(context_string, "rank");
	return cd;
}

static void unregister_function_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->function.func_name != NULL){
		free(cd->function.func_name);
	}
	free(cd);
}

static context_descriptor *register_func_call_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_FUNC_CALL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->func_call.func_name = get_param_str_value(context_string, "name1");
	cd->func_call.args_count = get_param_int_value(context_string, "rank");
	return cd;
}

static void unregister_func_call_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->func_call.func_name != NULL){
		free(cd->func_call.func_name);
	}
	free(cd);
}

static context_descriptor *register_interval_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_INTERVAL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

static void unregister_interval_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_master_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_MASTER;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

static void unregister_master_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_omploop_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
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
	return cd;
}

static void unregister_omploop_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->omploop.chunk_size != NULL){
		free(cd->omploop.chunk_size);
	}
	if (cd->omploop.names_private != NULL){
		list_destroy(cd->omploop.names_private, free);
	}
	if (cd->omploop.names_firstprivate != NULL){
		list_destroy(cd->omploop.names_firstprivate, free);
	}
	if (cd->omploop.names_lastprivate != NULL){
		list_destroy(cd->omploop.names_lastprivate, free);
	}
	if (cd->omploop.names_reduction != NULL){
		list_destroy(cd->omploop.names_reduction, free);
	}
	free(cd);
}

static context_descriptor *register_ordered_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_ORDERED;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

static void unregister_ordered_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_parallel_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
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
	return cd;
}

static void unregister_parallel_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->parallel.if_text != NULL){
		free(cd->parallel.if_text);
	}
	if (cd->parallel.num_threads != NULL){
		free(cd->parallel.num_threads);
	}
	if (cd->parallel.names_private != NULL){
		list_destroy(cd->parallel.names_private, free);
	}
	if (cd->parallel.names_shared != NULL){
		list_destroy(cd->parallel.names_shared, free);
	}
	if (cd->parallel.names_firstprivate != NULL){
		list_destroy(cd->parallel.names_firstprivate, free);
	}
	if (cd->parallel.names_copyin != NULL){
		list_destroy(cd->parallel.names_copyin, free);
	}
	if (cd->parallel.names_reduction != NULL){
		list_destroy(cd->parallel.names_reduction, free);
	}
	free(cd);
}

static context_descriptor *register_sections_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
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
	return cd;
}

static void unregister_sections_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->sections.names_private != NULL){
		list_destroy(cd->sections.names_private, free);
	}
	if (cd->sections.names_firstprivate != NULL){
		list_destroy(cd->sections.names_firstprivate, free);
	}
	if (cd->sections.names_lastprivate != NULL){
		list_destroy(cd->sections.names_lastprivate, free);
	}
	if (cd->sections.names_reduction != NULL){
		list_destroy(cd->sections.names_reduction, free);
	}
	free(cd);
}

static context_descriptor *register_section_event_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_SECTION_EVENT;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

static void unregister_section_event_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_seqloop_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_SEQLOOP;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

static void unregister_seqloop_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

static context_descriptor *register_single_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_SINGLE;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->single.is_nowait = get_param_int_value(context_string, "nowait");
	cd->single.names_private = get_param_names_list(context_string, "private");
	cd->single.names_firstprivate = get_param_names_list(context_string, "firstprivate");
	cd->single.names_copyprivate = get_param_names_list(context_string, "copyprivate");
	return cd;
}

static void unregister_single_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->single.names_private != NULL){
		list_destroy(cd->single.names_private, free);
	}
	if (cd->single.names_firstprivate != NULL){
		list_destroy(cd->single.names_firstprivate, free);
	}
	if (cd->single.names_copyprivate != NULL){
		list_destroy(cd->single.names_copyprivate, free);
	}
	free(cd);
}

static context_descriptor *register_threadprivate_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_THREADPRIVATE;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->threadprivate.names_threadprivate = get_param_names_list(context_string, "name1");
	return cd;
}

static void unregister_threadprivate_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->threadprivate.names_threadprivate != NULL){
		list_destroy(cd->threadprivate.names_threadprivate, free);
	}
	free(cd);
}

static context_descriptor *register_variable_name_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_VARIABLE_NAME;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	cd->var_name.var_name = get_param_str_value(context_string, "name1");
	cd->var_name.is_indata = get_param_int_value(context_string, "isindata");
	cd->var_name.is_incommon = get_param_int_value(context_string, "isincommon");
	cd->var_name.is_insave = get_param_int_value(context_string, "isinsave");
	cd->var_name.variable_type = get_variable_rt_type(context_string);
	return cd;
}

static void unregister_variable_name_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	if (cd->var_name.var_name != NULL){
		free(cd->var_name.var_name);
	}
	free(cd);
}

static context_descriptor *register_workshare_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_WORKSHARE;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	cd->workshare.is_nowait = get_param_int_value(context_string, "nowait");
	return cd;
}

static void unregister_workshare_context(context_descriptor *cd)
{
	if (cd->info.file_name != NULL){
		free(cd->info.file_name);
	}
	free(cd);
}

void unregister_contexts()
{
	list_iterator *it = list_iterator_new(registered_descriptors);
	while (list_iterator_has_next(it)){
		context_descriptor *cd = (context_descriptor *) list_iterator_next(it);
		unregister_context(cd);
	}
	list_iterator_destroy(it);
	list_destroy(registered_descriptors, NULL);
}

static void unregister_context(context_descriptor *cd)
{
	if (cd == NULL){
		return;
	}

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
		default:
            break;
	}
}