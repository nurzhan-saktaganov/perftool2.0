#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "context_string.h"
#include "context_descriptor.h"

// case insensitive string comparison
#define strcmp strcasecmp

static context_type get_context_type_from_name(const char *context_name)
{
	assert(context_name);

	if (strcmp(context_name, CTN_ARRAY_NAME) == 0){
		return CONTEXT_ARRAY_NAME;
	} else if (strcmp(context_name, CTN_BARRIER) == 0){
		return CONTEXT_BARRIER;
	} else if (strcmp(context_name, CTN_COMMON_NAME) == 0 ){
		return CONTEXT_COMMON_NAME;
	} else if (strcmp(context_name, CTN_CRITICAL) == 0){
		return CONTEXT_CRITICAL;
	} else if (strcmp(context_name, CTN_FILE_NAME) == 0){
		return CONTEXT_FILE_NAME;
	} else if (strcmp(context_name, CTN_FLUSH) == 0){
		return CONTEXT_FLUSH;
	} else if (strcmp(context_name, CTN_FUNCTION) == 0){
		return CONTEXT_FUNCTION;
	} else if (strcmp(context_name, CTN_FUNC_CALL) == 0){
		return CONTEXT_FUNC_CALL;
	} else if (strcmp(context_name, CTN_INTERVAL) == 0) {
		return CONTEXT_INTERVAL;
	} else if (strcmp(context_name, CTN_IO) == 0){
		return CONTEXT_IO;
	} else if (strcmp(context_name, CTN_MASTER) == 0){
		return CONTEXT_MASTER;
	} else if (strcmp(context_name, CTN_OMPLOOP) == 0){
		return CONTEXT_OMPLOOP;
	} else if (strcmp(context_name, CTN_ORDERED) == 0){
		return CONTEXT_ORDERED;
	} else if (strcmp(context_name, CTN_PARALLEL) == 0){
		return CONTEXT_PARALLEL;
	} else if (strcmp(context_name, CTN_SECTIONS) == 0){
		return CONTEXT_SECTIONS;
	} else if (strcmp(context_name, CTN_SECTION_EVENT) == 0){
		return CONTEXT_SECTION_EVENT;
	} else if (strcmp(context_name, CTN_SEQLOOP) == 0){
		return CONTEXT_SEQLOOP;
	} else if (strcmp(context_name, CTN_SINGLE) == 0){
		return CONTEXT_SINGLE;
	} else if (strcmp(context_name, CTN_THREADPRIVATE) == 0){
		return CONTEXT_THREADPRIVATE;
	} else if (strcmp(context_name, CTN_VARIABLE_NAME) == 0){
		return CONTEXT_VARIABLE_NAME;
	} else if (strcmp(context_name, CTN_WORKSHARE) == 0){
		return CONTEXT_WORKSHARE;
	}
	fprintf (stderr, "Unknown context name: \"%s\"\n", context_name);
	return CONTEXT_UNKNOWN;
}

context_type get_context_string_type(const char *context_string)
{
	char *context_type_name = get_param_str_value(context_string, "type");
	context_type type = get_context_type_from_name(context_type_name);
	free(context_type_name);
	return type;
}

static redop_type get_redop_type_from_name(const char *redop_name)
{
	assert(redop_name);

	if (strcmp(redop_name, RTN_PLUS) == 0){
		return REDOP_PLUS;
	} else if (strcmp(redop_name, RTN_MINUS) == 0){
		return REDOP_MINUS;
	} else if (strcmp(redop_name, RTN_PRODUCT) == 0){
		return REDOP_PRODUCT;
	} else if (strcmp(redop_name, RTN_AND) == 0){
		return REDOP_AND;
	} else if (strcmp(redop_name, RTN_OR) == 0){
		return REDOP_OR;
	} else if (strcmp(redop_name, RTN_EQV) == 0){
		return REDOP_EQV;
	} else if (strcmp(redop_name, RTN_NEQV) == 0){
		return REDOP_NEQV;
	} else if (strcmp(redop_name, RTN_MAX) == 0){
		return REDOP_MAX;
	} else if (strcmp(redop_name, RTN_MIN) == 0){
		return REDOP_MIN;
	} else if (strcmp(redop_name, RTN_IAND) == 0){
		return REDOP_IAND;
	} else if (strcmp(redop_name, RTN_IOR) == 0){
		return REDOP_IOR;
	} else if (strcmp(redop_name, RTN_IEOR) == 0){
		return REDOP_IEOR;
	}
	fprintf(stderr, "Unknown redop name: \"%s\"\n", redop_name);
	return REDOP_UNKNOWN;
}

redop_type get_redop_type(const char *context_string)
{
	char *redop_name = get_param_str_value(context_string, "redop");
	redop_type redop = get_redop_type_from_name(redop_name);
	free(redop_name);
	return redop;
}

static behavior_type get_behavior_type_from_name(const char *behavior_name)
{
	assert(behavior_name);

	if (strcmp(behavior_name, BTN_PRIVATE) == 0){
		return BEHAVIOR_PRIVATE;
	} else if (strcmp(behavior_name, BTN_SHARED) == 0){
		return BEHAVIOR_SHARED;
	} else if (strcmp(behavior_name, BTN_NONE) == 0){
		return BEHAVIOR_NONE;
	}
	fprintf(stderr, "Unknown behavior name: \"%s\"\n", behavior_name);
	return BEHAVIOR_UNKNOWN;
}

behavior_type get_behavior_type(const char *context_string)
{
	char *behavior_name = get_param_str_value(context_string, "default");
	behavior_type behavior = get_behavior_type_from_name(behavior_name);
	free(behavior_name);
	return behavior;
}

static schedule_type get_schedule_type_from_name(const char *schedule_name)
{
	assert(schedule_name);

	if (strcmp(schedule_name, STN_STATIC) == 0){
		return SCHEDULING_STATIC;
	} else if (strcmp(schedule_name, STN_DYNAMIC) == 0){
		return SCHEDULING_DYNAMIC;
	} else if (strcmp(schedule_name, STN_GUIDED) == 0) {
		return SCHEDULING_GUIDED;
	} else if (strcmp(schedule_name, STN_RUNTIME) == 0){
		return SCHEDULING_RUNTIME;
	}
	fprintf(stderr, "Unknown schedule name: \"%s\"\n", schedule_name);
	return SCHEDULING_UNKNOWN;
}

schedule_type get_schedule_type(const char *context_string)
{
	char *schedule_name = get_param_str_value(context_string, "schedule");
	schedule_type schedule = get_schedule_type_from_name(schedule_name);
	free(schedule_name);
	return schedule;
}

static variable_rt_type get_variable_rt_type_from_name(const char *rt_type_name)
{
	assert(rt_type_name);

	if (strcmp(rt_type_name, VTN_CHAR) == 0) {
		return RT_CHAR;
	} else if (strcmp(rt_type_name, VTN_INT) == 0) {
		return RT_INT;
	} else if (strcmp(rt_type_name, VTN_LONG) == 0) {
		return RT_LONG;
	} else if (strcmp(rt_type_name, VTN_FLOAT) == 0) {
		return RT_FLOAT;
	} else if (strcmp(rt_type_name, VTN_DOUBLE) == 0) {
		return RT_DOUBLE;
	} else if (strcmp(rt_type_name, VTN_FLOAT_COMPLEX) == 0) {
		return RT_FLOAT_COMPLEX;
	} else if (strcmp(rt_type_name, VTN_DOUBLE_COMPLEX) == 0) {
		return RT_DOUBLE_COMPLEX;
	}
	fprintf(stderr, "Unknown rt type name: \"%s\"\n", rt_type_name);
	return RT_UNKNOWN;
}

variable_rt_type get_variable_rt_type(const char *context_string)
{
	char *str_val = get_param_str_value(context_string, "vtype");
	variable_rt_type variable_type = get_variable_rt_type_from_name(str_val);
	free(str_val);
	return variable_type;
}
