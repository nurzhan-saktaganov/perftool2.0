#include <stdio.h>
#include <string.h>
#include "context_string.h"
#include "context_types.h"

context_type get_context_type_from_name(const char *context_name)
{
	if (context_name == NULL){
		return UNKNOWN_CONTEXT;
	} else if (strcmp(context_name, "arr_name") == 0){
		return ARRAY_NAME;
	} else if (strcmp(context_name, "barrier") == 0){
		return BARRIER;
	} else if (strcmp(context_name, "common_name") == 0 ){
		return COMMON_NAME;
	} else if (strcmp(context_name, "critical") == 0){
		return CRITICAL;
	} else if (strcmp(context_name, "file_name") == 0){
		return FILE_NAME;
	} else if (strcmp(context_name, "flush") == 0){
		return FLUSH;
	} else if (strcmp(context_name, "function") == 0){
		return FUNCTION;
	} else if (strcmp(context_name, "func_call") == 0){
		return FUNC_CALL;
	} else if (strcmp(context_name, "master") == 0){
		return MASTER;
	} else if (strcmp(context_name, "omploop") == 0){
		return OMPLOOP;
	} else if (strcmp(context_name, "ordered") == 0){
		return ORDERED;
	} else if (strcmp(context_name, "parallel") == 0){
		return PARALLEL;
	} else if (strcmp(context_name, "sections") == 0){
		return SECTIONS;
	} else if (strcmp(context_name, "sect_ev") == 0){
		return SECTION_EVENT;
	} else if (strcmp(context_name, "seqloop") == 0){
		return SEQLOOP;
	} else if (strcmp(context_name, "single") == 0){
		return SINGLE;
	} else if (strcmp(context_name, "threadprivate") == 0){
		return THREADPRIVATE;
	} else if (strcmp(context_name, "var_name") == 0){
		return VARIABLE_NAME;
	} else if (strcmp(context_name, "workshare") == 0){
		return WORKSHARE;
	}
	fprintf (stderr, "Unknown context name: %s\n", context_name); 
	return UNKNOWN_CONTEXT;
}

context_type get_context_string_type(const char *context_string)
{
	context_type type;
	char *context_type_name = get_param_str_value(context_string, "type");
	type = get_context_type_from_name(context_type_name);
	if (context_type_name != NULL){
		free(context_type_name);
	}
	return type;
}

redop_type get_redop_type_from_name(const char *redop_name)
{
	if (redop_name == NULL){
		return UNKNOWN_REDOP;
	} else if (strcmp(redop_name, "PLUS") == 0){
		return PLUS;
	} else if (strcmp(redop_name, "MINUS") == 0){
		return MINUS;
	} else if (strcmp(redop_name, "PRODUCT") == 0){
		return PRODUCT;
	} else if (strcmp(redop_name, "AND") == 0){
		return AND;
	} else if (strcmp(redop_name, "OR") == 0){
		return OR;
	} else if (strcmp(redop_name, "EQV") == 0){
		return EQV;
	} else if (strcmp(redop_name, "NEQV") == 0){
		return NEQV;
	} else if (strcmp(redop_name, "MAX") == 0){
		return MAX;
	} else if (strcmp(redop_name, "MIN") == 0){
		return MIN;
	} else if (strcmp(redop_name, "IAND") == 0){
		return IAND;
	} else if (strcmp(redop_name, "IOR") == 0){
		return IOR;
	} else if (strcmp(redop_name, "IEOR") == 0){
		return IEOR;
	}
	fprintf(stderr, "Unknown redop name: %s\n", redop_name);
	return UNKNOWN_REDOP;
}

redop_type get_redop_type(const char *context_string)
{
	redop_type redop;
	char *redop_name = get_param_str_value(context_string, "redop");
	redop = get_redop_type_from_name(redop_name);
	if (redop_name != NULL){
		free(redop_name);
	}
	return redop;
}

behavior_type get_behavior_type_from_name(const char *behavior_name)
{
	if (behavior_name == NULL){
		return UNKNOWN_BEHAVIOR;
	} else if (strcmp(behavior_name, "PRIVATE") == 0){
		return PRIVATE;
	} else if (strcmp(behavior_name, "SHARED") == 0){
		return SHARED;
	} else if (strcmp(behavior_name, "NONE") == 0){
		return NONE;
	}
	fprintf(stderr, "Unknown behavior name: %s\n", behavior_name);
	return UNKNOWN_BEHAVIOR;
}

behavior_type get_behavior_type(const char *context_string)
{
	behavior_type behavior;
	char *behavior_name = get_param_str_value(context_string, "default");
	behavior = get_behavior_type_from_name(behavior_name);
	if (behavior_name != NULL){
		free(behavior_name);
	}
	return behavior;
}

schedule_type get_schedule_type_from_name(const char *schedule_name)
{
	if (schedule_name == NULL){
		return UNKNOWN_SCHEDULING;
	} else if (strcmp(schedule_name, "STATIC") == 0){
		return STATIC;
	} else if (strcmp(schedule_name, "DYNAMIC") == 0){
		return DYNAMIC;
	} else if (strcmp(schedule_name, "GUIDED") == 0) {
		return GUIDED;
	} else if (strcmp(schedule_name, "RUNTIME") == 0){
		return RUNTIME;
	}
	fprintf(stderr, "Unknown schedule name: %s\n", schedule_name);
	return UNKNOWN_SCHEDULING;
}

schedule_type get_schedule_type(const char *context_string)
{
	schedule_type schedule;
	char *schedule_name = get_param_str_value(context_string, "schedule");
	schedule = get_schedule_type_from_name(schedule_name);
	if (schedule_name != NULL){
		free(schedule_name);
	}
	return schedule;
}

variable_rt_type get_variable_rt_type_from_name(const char *rt_type_name)
{
	if (rt_type_name == NULL) {
		return UNKNOWN_RT_TYPE;
	} else if (strcmp(rt_type_name, "0") == 0) {
		return RT_CHAR;
	} else if (strcmp(rt_type_name, "1") == 0) {
		return RT_INT;
	} else if (strcmp(rt_type_name, "2") == 0) {
		return RT_LONG;
	} else if (strcmp(rt_type_name, "3") == 0) {
		return RT_FLOAT;
	} else if (strcmp(rt_type_name, "4") == 0) {
		return RT_DOUBLE;
	} else if (strcmp(rt_type_name, "5") == 0) {
		return RT_FLOAT_COMPLEX;
	} else if (strcmp(rt_type_name, "6") == 0) {
		return RT_DOUBLE_COMPLEX;
	}
	fprintf(stderr, "Unknown rt type name: %s\n", rt_type_name);
	return UNKNOWN_RT_TYPE;
}

variable_rt_type get_variable_rt_type(const char *context_string)
{
	variable_rt_type variable_type;
	char *str_val = get_param_str_value(context_string, "vtype");
	variable_type = get_variable_rt_type_from_name(str_val);
	if (str_val != NULL){
		free(str_val);
	}
	return variable_type;
}

/*
const char * get_context_type_name(context_type type)
{
	switch (type){
		case ARRAY_NAME:
			return "arr_name";
		case BARRIER:
			return "barrier";
		case COMMON_NAME:
			return "common_name";
		case CRITICAL:
			return "critical";
		case FILENAME:
			return "filename";
		case FLUSH:
			return "flush";
		case FUNCTION:
			return "function";
		case FUNC_CALL:
			return "func_call";
		case MASTER:
			return "master";
		case OMPLOOP:
			return "omploop";
		case ORDERED:
			return "ordered";
		case PARALLEL:
			return "parallel";
		case SECTIONS:
			return "sections";
		case SECTION_EVENT:
			return "sect_ev";
		case SEQLOOP:
			return "seqloop";
		case SINGLE:
			return "single";
		case THREADPRIVATE:
			return "threadprivate";
		case UNKNOWN_CONTEXT:
			return "unknown";
		case VARIABLE_NAME:
			return "var_name";
		case WORKSHARE:
			return "workshare";
		default:
			return "unknown";
	}	
}
*/
