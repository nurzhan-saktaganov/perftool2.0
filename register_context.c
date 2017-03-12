#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "context_string.h"
#include "context_types.h"
#include "register_context.h"


array_name_context_descriptor *register_array_name_context(const char *context_string);
barrier_context_descriptor *register_barrier_context(const char *context_string);
common_name_context_descriptor *register_common_name_context(const char *context_string);
critical_context_descriptor *register_critical_context(const char *context_string);
file_name_context_descriptor *register_filename_context(const char *context_string);
flush_context_descriptor *register_flush_context(const char *context_string);
function_context_descriptor *register_function_context(const char *context_string);
func_call_context_descriptor *register_func_call_context(const char *context_string);
master_context_descriptor *register_master_context(const char *context_string);
omploop_context_descriptor *register_omploop_context(const char *context_string);
ordered_context_descriptor *register_ordered_context(const char *context_string);
parallel_context_descriptor *register_parallel_context(const char *context_string);
sections_context_descriptor *register_sections_context(const char *context_string);
section_event_context_descriptor *register_section_event_context(const char *context_string);
seqloop_context_descriptor *register_seqloop_context(const char *context_string);
single_context_descriptor *register_single_context(const char *context_string);
threadprivate_context_descriptor *register_threadprivate_context(const char *context_string);
variable_name_context_descriptor *register_variable_name_context(const char *context_string);
workshare_context_descriptor *register_workshare_context(const char *context_string);


long register_context(const char *context_string)
{
	context_type type;
	void *context_ptr;
	context_descriptor *context_desc;
	
	type = get_context_string_type(context_string);
	switch (type){
		case ARRAY_NAME:
			context_ptr = (void *) register_array_name_context(context_string);
			break;
		case BARRIER:
			context_ptr = (void *) register_barrier_context(context_string);
			break;
		case COMMON_NAME:
			context_ptr = (void *) register_common_name_context(context_string);
			break;
		case CRITICAL:
			context_ptr = (void *) register_critical_context(context_string);
			break;
		case FILE_NAME:
			context_ptr = (void *) register_filename_context(context_string);
			break;
		case FLUSH:
			context_ptr = (void *) register_flush_context(context_string);
			break;
		case FUNCTION:
			context_ptr = (void *) register_function_context(context_string);
			break;
		case FUNC_CALL:
			context_ptr = (void *) register_func_call_context(context_string);
			break;
		case MASTER:
			context_ptr = (void *) register_master_context(context_string);
			break;
		case OMPLOOP:
			context_ptr = (void *) register_omploop_context(context_string);
			break;
		case ORDERED:
			context_ptr = (void *) register_ordered_context(context_string);
			break;
		case PARALLEL:
			context_ptr = (void *) register_parallel_context(context_string);
			break;
		case SECTIONS:
			context_ptr = (void *) register_sections_context(context_string);
			break;
		case SECTION_EVENT:
			context_ptr = (void *) register_section_event_context(context_string);
			break;
		case SEQLOOP:
			context_ptr = (void *) register_seqloop_context(context_string);
			break;
		case SINGLE:
			context_ptr = (void *) register_single_context(context_string);
			break;
		case THREADPRIVATE:
			context_ptr = (void *) register_threadprivate_context(context_string);
			break;
		case UNKNOWN_CONTEXT:
			context_ptr = (void *) NULL;
			break;
		case VARIABLE_NAME:
			context_ptr = (void *) register_variable_name_context(context_string);
			break;
		case WORKSHARE:
			context_ptr = (void *) register_workshare_context(context_string);
			break;
		default:
			context_ptr = (void *) NULL;
			break;
	}
	
	if (context_ptr == NULL){
		fprintf(stderr, "Unknown context string format: %s\n", context_string);
		context_desc = NULL;
	} else {
		context_desc = (context_descriptor *) malloc(sizeof(context_descriptor));
		context_desc->type = type;
		context_desc->context_ptr = context_ptr;
	}
	
	return (long) context_desc;
}


array_name_context_descriptor *register_array_name_context(const char *context_string)
{
	array_name_context_descriptor *context;
	context = (array_name_context_descriptor *) malloc(sizeof(array_name_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->arr_name = get_param_str_value(context_string, "name1");
	context->variable_type = get_variable_rt_type(context_string);
	context->rank = get_param_int_value(context_string, "rank");
	context->is_indata = get_param_int_value(context_string, "isindata");
	context->is_incommon = get_param_int_value(context_string, "isincommon");
	context->is_insave = get_param_int_value(context_string, "isinsave");
	return context;
}


barrier_context_descriptor *register_barrier_context(const char *context_string)
{
	barrier_context_descriptor *context;
	context = (barrier_context_descriptor *) malloc(sizeof(barrier_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	return context;
}


common_name_context_descriptor *register_common_name_context(const char *context_string)
{
	common_name_context_descriptor *context;
	context = (common_name_context_descriptor *) malloc(sizeof(common_name_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->block_name = get_param_str_value(context_string, "name1");
	context->components = get_param_names_list(context_string, "name2");
	return context;
}


critical_context_descriptor *register_critical_context(const char *context_string)
{
	critical_context_descriptor *context;
	context = (critical_context_descriptor *) malloc(sizeof(critical_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->critical_name = get_param_str_value(context_string, "name1");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


file_name_context_descriptor *register_filename_context(const char *context_string)
{
	file_name_context_descriptor *context;
	context = (file_name_context_descriptor *) malloc(sizeof(file_name_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	return context;
}


flush_context_descriptor *register_flush_context(const char *context_string)
{
	flush_context_descriptor *context;
	context = (flush_context_descriptor *) malloc (sizeof(flush_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->flushed = get_param_names_list(context_string, "name1");
	return context;
}


function_context_descriptor *register_function_context(const char *context_string)
{
	function_context_descriptor *context;
	context = (function_context_descriptor *) malloc(sizeof(function_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->func_name = get_param_str_value(context_string, "name1");
	context->args_count = get_param_int_value(context_string, "rank");
	return context;
}


func_call_context_descriptor *register_func_call_context(const char *context_string)
{
	func_call_context_descriptor *context;
	context = (func_call_context_descriptor *) malloc(sizeof(func_call_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->func_name = get_param_str_value(context_string, "name1");
	context->args_count = get_param_int_value(context_string, "rank");
	return context;
}


master_context_descriptor *register_master_context(const char *context_string)
{
	master_context_descriptor *context;
	context = (master_context_descriptor *) malloc(sizeof(master_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


omploop_context_descriptor *register_omploop_context(const char *context_string)
{
	omploop_context_descriptor *context;
	context = (omploop_context_descriptor *) malloc(sizeof(omploop_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_ordered = get_param_int_value(context_string, "ordered");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	context->private = get_param_names_list(context_string, "private");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->lastprivate = get_param_names_list(context_string, "lastprivate");
	context->reduction = get_param_names_list(context_string, "reduction");
	context->redop = get_redop_type(context_string);
	context->schedule = get_schedule_type(context_string);
	context->chunk_size = get_param_str_value(context_string, "chunk_size");
	return context;
}


ordered_context_descriptor *register_ordered_context(const char *context_string)
{
	ordered_context_descriptor *context;
	context = (ordered_context_descriptor *) malloc(sizeof(ordered_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


parallel_context_descriptor *register_parallel_context(const char *context_string)
{
	parallel_context_descriptor *context;
	context = (parallel_context_descriptor *) malloc(sizeof(parallel_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->private = get_param_names_list(context_string, "private");
	context->shared = get_param_names_list(context_string, "shared");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->copyin = get_param_names_list(context_string, "copyin");
	context->reduction = get_param_names_list(context_string, "reduction");
	context->redop = get_redop_type(context_string);
	context->default_behavior = get_behavior_type(context_string);
	context->if_text = get_param_str_value(context_string, "if");
	context->num_threads = get_param_str_value(context_string, "num_threads");
	return context;
}


sections_context_descriptor *register_sections_context(const char *context_string)
{
	sections_context_descriptor *context;
	context = (sections_context_descriptor *) malloc(sizeof(sections_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	context->private = get_param_names_list(context_string, "private");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->lastprivate = get_param_names_list(context_string, "lastprivate");
	context->reduction = get_param_names_list(context_string, "reduction");
	context->redop = get_redop_type(context_string);
	return context;
}


section_event_context_descriptor *register_section_event_context(const char *context_string)
{
	section_event_context_descriptor *context;
	context = (section_event_context_descriptor *) malloc(sizeof(section_event_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


seqloop_context_descriptor *register_seqloop_context(const char *context_string)
{
	seqloop_context_descriptor *context;
	context = (seqloop_context_descriptor *) malloc(sizeof(seqloop_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


single_context_descriptor *register_single_context(const char *context_string)
{
	single_context_descriptor *context;
	context = (single_context_descriptor *) malloc(sizeof(single_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	context->private = get_param_names_list(context_string, "private");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->copyprivate = get_param_names_list(context_string, "copyprivate");
	return context;
}


threadprivate_context_descriptor *register_threadprivate_context(const char *context_string)
{
	threadprivate_context_descriptor *context;
	context = (threadprivate_context_descriptor *) malloc(sizeof(threadprivate_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->threadprivate = get_param_names_list(context_string, "name1");
	return context;
}


variable_name_context_descriptor *register_variable_name_context(const char *context_string)
{
	variable_name_context_descriptor *context;
	context = (variable_name_context_descriptor *) malloc(sizeof(variable_name_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->var_name = get_param_str_value(context_string, "name1");
	context->is_indata = get_param_int_value(context_string, "isindata");
	context->is_incommon = get_param_int_value(context_string, "isincommon");
	context->is_insave = get_param_int_value(context_string, "isinsave");
	context->variable_type = get_variable_rt_type(context_string);
	return context;
}


workshare_context_descriptor *register_workshare_context(const char *context_string)
{
	workshare_context_descriptor *context;
	context = (workshare_context_descriptor *) malloc(sizeof(workshare_context_descriptor));
	context->file_name = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	return context;
}
