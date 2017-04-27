#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "context_string.h"
#include "context_descriptor.h"
#include "register_context.h"

context_descriptor *register_array_name_context(const char *context_string);
context_descriptor *register_barrier_context(const char *context_string);
context_descriptor *register_common_name_context(const char *context_string);
context_descriptor *register_critical_context(const char *context_string);
context_descriptor *register_filename_context(const char *context_string);
context_descriptor *register_flush_context(const char *context_string);
context_descriptor *register_function_context(const char *context_string);
context_descriptor *register_func_call_context(const char *context_string);
context_descriptor *register_interval_context(const char *context_string);
context_descriptor *register_master_context(const char *context_string);
context_descriptor *register_omploop_context(const char *context_string);
context_descriptor *register_ordered_context(const char *context_string);
context_descriptor *register_parallel_context(const char *context_string);
context_descriptor *register_sections_context(const char *context_string);
context_descriptor *register_section_event_context(const char *context_string);
context_descriptor *register_seqloop_context(const char *context_string);
context_descriptor *register_single_context(const char *context_string);
context_descriptor *register_threadprivate_context(const char *context_string);
context_descriptor *register_variable_name_context(const char *context_string);
context_descriptor *register_workshare_context(const char *context_string);

context_descriptor * register_context(const char *context_string)
{
	context_type type = get_context_string_type(context_string);
	switch (type){
		case CONTEXT_ARRAY_NAME:
		    return register_array_name_context(context_string);
		case CONTEXT_BARRIER:
		    return register_barrier_context(context_string);
		case CONTEXT_COMMON_NAME:
			return register_common_name_context(context_string);
		case CONTEXT_CRITICAL:
			return register_critical_context(context_string);
		case CONTEXT_FILE_NAME:
			return register_filename_context(context_string);
		case CONTEXT_FLUSH:
			return register_flush_context(context_string);
		case CONTEXT_FUNCTION:
			return register_function_context(context_string);
		case CONTEXT_FUNC_CALL:
			return register_func_call_context(context_string);
		case CONTEXT_INTERVAL:
			return register_interval_context(context_string);
		case CONTEXT_MASTER:
			return register_master_context(context_string);
		case CONTEXT_OMPLOOP:
			return register_omploop_context(context_string);
		case CONTEXT_ORDERED:
			return register_ordered_context(context_string);
		case CONTEXT_PARALLEL:
			return register_parallel_context(context_string);
		case CONTEXT_SECTIONS:
			return register_sections_context(context_string);
		case CONTEXT_SECTION_EVENT:
			return register_section_event_context(context_string);
		case CONTEXT_SEQLOOP:
			return register_seqloop_context(context_string);
		case CONTEXT_SINGLE:
			return register_single_context(context_string);
		case CONTEXT_THREADPRIVATE:
			return register_threadprivate_context(context_string);
		case CONTEXT_UNKNOWN:
			break;
		case CONTEXT_VARIABLE_NAME:
			return register_variable_name_context(context_string);
		case CONTEXT_WORKSHARE:
			return register_workshare_context(context_string);
		default:
            break;
	}
	fprintf(stderr, "Unknown context string format: %s\n", context_string);	
	return NULL;
}

context_descriptor *register_array_name_context(const char *context_string)
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

context_descriptor *register_barrier_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_BARRIER;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	return cd;
}

context_descriptor *register_common_name_context(const char *context_string)
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

context_descriptor *register_critical_context(const char *context_string)
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

context_descriptor *register_filename_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_FILE_NAME;
    cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = cd->info.begin_line;
	return cd;
}

context_descriptor *register_flush_context(const char *context_string)
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

context_descriptor *register_function_context(const char *context_string)
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

context_descriptor *register_func_call_context(const char *context_string)
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

context_descriptor *register_interval_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_INTERVAL;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

context_descriptor *register_master_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_MASTER;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

context_descriptor *register_omploop_context(const char *context_string)
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

context_descriptor *register_ordered_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_ORDERED;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

context_descriptor *register_parallel_context(const char *context_string)
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

context_descriptor *register_sections_context(const char *context_string)
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

context_descriptor *register_section_event_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_SECTION_EVENT;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

context_descriptor *register_seqloop_context(const char *context_string)
{
    context_descriptor *cd = (context_descriptor *) malloc(sizeof(context_descriptor));
	assert(cd);
    cd->info.type = CONTEXT_SEQLOOP;
	cd->info.file_name = get_param_str_value(context_string, "file");
	cd->info.begin_line = get_param_int_value(context_string, "line1");
	cd->info.end_line = get_param_int_value(context_string, "line2");
	return cd;
}

context_descriptor *register_single_context(const char *context_string)
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

context_descriptor *register_threadprivate_context(const char *context_string)
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

context_descriptor *register_variable_name_context(const char *context_string)
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

context_descriptor *register_workshare_context(const char *context_string)
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
