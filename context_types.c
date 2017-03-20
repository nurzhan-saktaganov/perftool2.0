#include <stdio.h>
#include <string.h>
#include "context_string.h"
#include "context_types.h"

context_type get_context_type_from_name(const char *context_name)
{
	if (context_name == NULL){
		return CONTEXT_UNKNOWN;
	} else if (strcmp(context_name, "arr_name") == 0){
		return CONTEXT_ARRAY_NAME;
	} else if (strcmp(context_name, "barrier") == 0){
		return CONTEXT_BARRIER;
	} else if (strcmp(context_name, "common_name") == 0 ){
		return CONTEXT_COMMON_NAME;
	} else if (strcmp(context_name, "critical") == 0){
		return CONTEXT_CRITICAL;
	} else if (strcmp(context_name, "file_name") == 0){
		return CONTEXT_FILE_NAME;
	} else if (strcmp(context_name, "flush") == 0){
		return CONTEXT_FLUSH;
	} else if (strcmp(context_name, "function") == 0){
		return CONTEXT_FUNCTION;
	} else if (strcmp(context_name, "func_call") == 0){
		return CONTEXT_FUNC_CALL;
	} else if (strcmp(context_name, "master") == 0){
		return CONTEXT_MASTER;
	} else if (strcmp(context_name, "omploop") == 0){
		return CONTEXT_OMPLOOP;
	} else if (strcmp(context_name, "ordered") == 0){
		return CONTEXT_ORDERED;
	} else if (strcmp(context_name, "parallel") == 0){
		return CONTEXT_PARALLEL;
	} else if (strcmp(context_name, "sections") == 0){
		return CONTEXT_SECTIONS;
	} else if (strcmp(context_name, "sect_ev") == 0){
		return CONTEXT_SECTION_EVENT;
	} else if (strcmp(context_name, "seqloop") == 0){
		return CONTEXT_SEQLOOP;
	} else if (strcmp(context_name, "single") == 0){
		return CONTEXT_SINGLE;
	} else if (strcmp(context_name, "threadprivate") == 0){
		return CONTEXT_THREADPRIVATE;
	} else if (strcmp(context_name, "var_name") == 0){
		return CONTEXT_VARIABLE_NAME;
	} else if (strcmp(context_name, "workshare") == 0){
		return CONTEXT_WORKSHARE;
	}
	fprintf (stderr, "Unknown context name: %s\n", context_name); 
	return CONTEXT_UNKNOWN;
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
		return REDOP_UNKNOWN;
	} else if (strcmp(redop_name, "PLUS") == 0){
		return REDOP_PLUS;
	} else if (strcmp(redop_name, "MINUS") == 0){
		return REDOP_MINUS;
	} else if (strcmp(redop_name, "PRODUCT") == 0){
		return REDOP_PRODUCT;
	} else if (strcmp(redop_name, "AND") == 0){
		return REDOP_AND;
	} else if (strcmp(redop_name, "OR") == 0){
		return REDOP_OR;
	} else if (strcmp(redop_name, "EQV") == 0){
		return REDOP_EQV;
	} else if (strcmp(redop_name, "NEQV") == 0){
		return REDOP_NEQV;
	} else if (strcmp(redop_name, "MAX") == 0){
		return REDOP_MAX;
	} else if (strcmp(redop_name, "MIN") == 0){
		return REDOP_MIN;
	} else if (strcmp(redop_name, "IAND") == 0){
		return REDOP_IAND;
	} else if (strcmp(redop_name, "IOR") == 0){
		return REDOP_IOR;
	} else if (strcmp(redop_name, "IEOR") == 0){
		return REDOP_IEOR;
	}
	fprintf(stderr, "Unknown redop name: %s\n", redop_name);
	return REDOP_UNKNOWN;
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
		return BEHAVIOR_UNKNOWN;
	} else if (strcmp(behavior_name, "PRIVATE") == 0){
		return BEHAVIOR_PRIVATE;
	} else if (strcmp(behavior_name, "SHARED") == 0){
		return BEHAVIOR_SHARED;
	} else if (strcmp(behavior_name, "NONE") == 0){
		return BEHAVIOR_NONE;
	}
	fprintf(stderr, "Unknown behavior name: %s\n", behavior_name);
	return BEHAVIOR_UNKNOWN;
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
		return SCHEDULING_UNKNOWN;
	} else if (strcmp(schedule_name, "STATIC") == 0){
		return SCHEDULING_STATIC;
	} else if (strcmp(schedule_name, "DYNAMIC") == 0){
		return SCHEDULING_DYNAMIC;
	} else if (strcmp(schedule_name, "GUIDED") == 0) {
		return SCHEDULING_GUIDED;
	} else if (strcmp(schedule_name, "RUNTIME") == 0){
		return SCHEDULING_RUNTIME;
	}
	fprintf(stderr, "Unknown schedule name: %s\n", schedule_name);
	return SCHEDULING_UNKNOWN;
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
		return RT_UNKNOWN;
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
	return RT_UNKNOWN;
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
