#include <string.h>
#include <stdlib.h>
#include "context_string.h"
#include "context_types.h"
#include "register_context.h"

array_name_context *register_array_name_context(const char *context_string);
barrier_context *register_barrier_context(const char *context_string);
common_name_context *register_common_name_context(const char *context_string);
critical_context *register_critical_context(const char *context_string);
file_name_context *register_filename_context(const char *context_string);
flush_context *register_flush_context(const char *context_string);
function_context *register_function_context(const char *context_string);
func_call_context *register_func_call_context(const char *context_string);
master_context *register_master_context(const char *context_string);
omploop_context *register_omploop_context(const char *context_string);
ordered_context *register_ordered_context(const char *context_string);
parallel_context *register_parallel_context(const char *context_string);
sections_context *register_sections_context(const char *context_string);
section_event_context *register_section_event_context(const char *context_string);
seqloop_context *register_seqloop_context(const char *context_string);
single_context *register_single_context(const char *context_string);
threadprivate_context *register_threadprivate_context(const char *context_string);
variable_name_context *register_variable_name_context(const char *context_string);
workshare_context *register_workshare_context(const char *context_string);

long register_context(const char *context_string)
{
	context_type type;
	void *context_ptr;
	context_descriptor *context_desc;
	
	type = get_context_string_type(context_string);
	switch (type){
		case ARRAY_NAME:
			context_ptr = (void *) register_array_name_context(context_string);
		case BARRIER:
			context_ptr = (void *) register_barrier_context(context_string);
		case COMMON_NAME:
			context_ptr = (void *) register_common_name_context(context_string);
		case CRITICAL:
			context_ptr = (void *) register_critical_context(context_string);
		case FILENAME:
			context_ptr = (void *) register_filename_context(context_string);
		case FLUSH:
			context_ptr = (void *) register_flush_context(context_string);
		case FUNCTION:
			context_ptr = (void *) register_function_context(context_string);
		case FUNC_CALL:
			context_ptr = (void *) register_func_call_context(context_string);
		case MASTER:
			context_ptr = (void *) register_master_context(context_string);
		case OMPLOOP:
			context_ptr = (void *) register_omploop_context(context_string);
		case ORDERED:
			context_ptr = (void *) register_ordered_context(context_string);
		case PARALLEL:
			context_ptr = (void *) register_parallel_context(context_string);
		case SECTIONS:
			context_ptr = (void *) register_sections_context(context_string);
		case SECTION_EVENT:
			context_ptr = (void *) register_section_event_context(context_string);
		case SEQLOOP:
			context_ptr = (void *) register_seqloop_context(context_string);
		case SINGLE:
			context_ptr = (void *) register_single_context(context_string);
		case THREADPRIVATE:
			context_ptr = (void *) register_threadprivate_context(context_string);
		case UNKNOWN_CONTEXT:
			context_ptr = (void *) NULL;
		case VARIABLE_NAME:
			context_ptr = (void *) register_variable_name_context(context_string);
		case WORKSHARE:
			context_ptr = (void *) register_workshare_context(context_string);
		default:
			context_ptr = (void *) NULL;
	}
	
	if (context_ptr == NULL){
		
		context_desc = NULL;
	} else {
		context_desc = (context_descriptor *) malloc(sizeof(context_descriptor));
		context_desc->type = type;
		context_desc->context_ptr = context_ptr;
	}
	
	return (long) context_desc;
}

array_name_context *register_array_name_context(const char *context_string)
{
	array_name_context *context;
	context = (array_name_context *) malloc(sizeof(array_name_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->arr_name = get_param_str_value(context_string, "name1");
	context->variable_type = get_param_int_value(context_string, "vtype");
	context->rank = get_param_int_value(context_string, "rank");
	context->is_indata = get_param_int_value(context_string, "isindata");
	context->is_incommon = get_param_int_value(context_string, "isincommon");
	context->is_insave = get_param_int_value(context_string, "isinsave");
	return context;
}


barrier_context *register_barrier_context(const char *context_string)
{
	barrier_context *context;
	context = (barrier_context *) malloc(sizeof(barrier_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	return context;
}


common_name_context *register_common_name_context(const char *context_string)
{
	common_name_context *context;	
	context = (common_name_context *) malloc(sizeof(common_name_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->block_name = get_param_str_value(context_string, "name1");
	context->components = get_param_names_list(context_string, "name2");
	return context;
}


critical_context *register_critical_context(const char *context_string)
{
	critical_context *context;
	context = (critical_context *) malloc(sizeof(critical_context));
	context->filename = get_param_str_value(context_string, "file");
	context->name = get_param_str_value(context_string, "name1");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


file_name_context *register_filename_context(const char *context_string)
{
	file_name_context *context;
	context = (file_name_context *) malloc(sizeof(file_name_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	return context;
}


flush_context *register_flush_context(const char *context_string)
{
	flush_context *context;
	context = (flush_context *) malloc (sizeof(flush_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->flushed = get_param_names_list(context_string, "name1");
	return context;
}


function_context *register_function_context(const char *context_string)
{
	function_context *context;
	context = (function_context *) malloc(sizeof(function_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->func_name = get_param_str_value(context_string, "name1");
	context->args_count = get_param_int_value(context_string, "rank");
	return context;
}


func_call_context *register_func_call_context(const char *context_string)
{
	func_call_context *context;
	context = (func_call_context *) malloc(sizeof(func_call_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->name = get_param_str_value(context_string, "name1");
	context->args_count = get_param_int_value(context_string, "rank");
	return context;
}


master_context *register_master_context(const char *context_string)
{
	master_context *context;
	context = (master_context *) malloc(sizeof(master_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


omploop_context *register_omploop_context(const char *context_string)
{
	omploop_context *context;
	char *str_val;
	
	context = (omploop_context *) malloc(sizeof(omploop_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_ordered = get_param_int_value(context_string, "ordered");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	context->private = get_param_names_list(context_string, "private");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->lastprivate = get_param_names_list(context_string, "lastprivate");
	context->reduction = get_param_names_list(context_string, "reduction");
	
	// Получение типа редукционной операции
	str_val = get_param_str_value(context_string, "redop");
	context->redop = get_redop_type_from_name(str_val);
	if (str_val != NULL){
		free(str_val);
	}
	
	// Получение вида scheduling
	str_val = get_param_str_value(context_string, "schedule");
	context->schedule = get_schedule_type_from_name(str_val);
	if (str_val != NULL){
		free(str_val);
	}
	
	context->chunk_size = get_param_str_value(context_string, "chunk_size");
	return context;
}


ordered_context *register_ordered_context(const char *context_string)
{
	ordered_context *context;
	context = (ordered_context *) malloc(sizeof(ordered_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


parallel_context *register_parallel_context(const char *context_string)
{
	parallel_context *context;
	char *str_val;
	
	context = (parallel_context *) malloc(sizeof(parallel_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->private = get_param_names_list(context_string, "private");
	context->shared = get_param_names_list(context_string, "shared");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->copyin = get_param_names_list(context_string, "copyin");
	context->reduction = get_param_names_list(context_string, "reduction");
	
	// Получение типа редукционной операции
	str_val = get_param_str_value(context_string, "redop");
	context->redop = get_redop_type_from_name(str_val);
	if (str_val != NULL){
		free(str_val);
	}
	
	str_val = get_param_str_value(context_string, "default");
	context->default_behavior = get_behavior_type_from_name(str_val);
	if (str_val != NULL){
		free(str_val);
	}
	
	context->if_text = get_param_str_value(context_string, "if");
	context->num_threads = get_param_str_value(context_string, "num_threads");
	return context;
}


sections_context *register_sections_context(const char *context_string)
{
	sections_context *context;
	char *str_val;
	
	context = (sections_context *) malloc(sizeof(sections_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	context->private = get_param_names_list(context_string, "private");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->lastprivate = get_param_names_list(context_string, "lastprivate");
	context->reduction = get_param_names_list(context_string, "reduction");
	
	// Получение типа редукционной операции
	str_val = get_param_str_value(context_string, "redop");
	context->redop = get_redop_type_from_name(str_val);
	if (str_val != NULL){
		free(str_val);
	}
	
	return context;
}


section_event_context *register_section_event_context(const char *context_string)
{
	section_event_context *context;
	context = (section_event_context *) malloc(sizeof(section_event_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


seqloop_context *register_seqloop_context(const char *context_string)
{
	seqloop_context *context;
	context = (seqloop_context *) malloc(sizeof(seqloop_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	return context;
}


single_context *register_single_context(const char *context_string)
{
	single_context *context;
	context = (single_context *) malloc(sizeof(single_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	context->private = get_param_names_list(context_string, "private");
	context->firstprivate = get_param_names_list(context_string, "firstprivate");
	context->copyprivate = get_param_names_list(context_string, "copyprivate");
	return context;
}


threadprivate_context *register_threadprivate_context(const char *context_string)
{
	threadprivate_context *context;
	context = (threadprivate_context *) malloc(sizeof(threadprivate_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->threadprivate = get_param_names_list(context_string, "name1");
	return context;
}


variable_name_context *register_variable_name_context(const char *context_string)
{
	variable_name_context *context;
	char *str_val;
	context = (variable_name_context *) malloc(sizeof(variable_name_context));
	context->filename = get_param_str_value(context_string, "file");
	context->line_number = get_param_int_value(context_string, "line1");
	context->var_name = get_param_str_value(context_string, "name1");
	context->is_indata = get_param_int_value(context_string, "isindata");
	context->is_incommon = get_param_int_value(context_string, "isincommon");
	context->is_insave = get_param_int_value(context_string, "isinsave");
	
	// Получение типа переменной
	str_val = get_param_str_value(context_string, "vtype");
	context->variable_type = get_variable_rt_type_from_name(str_val);
	if (str_val != NULL) {
		free(str_val);
	}
	return context;
}


workshare_context *register_workshare_context(const char *context_string)
{
	workshare_context *context;
	context = (workshare_context *) malloc(sizeof(workshare_context));
	context->filename = get_param_str_value(context_string, "file");
	context->begin_line = get_param_int_value(context_string, "line1");
	context->end_line = get_param_int_value(context_string, "line2");
	context->is_nowait = get_param_int_value(context_string, "nowait");
	return context;
}
