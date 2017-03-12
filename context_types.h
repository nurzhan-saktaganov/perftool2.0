#ifndef CONTEXT_TYPES_H
#define CONTEXT_TYPES_H

typedef enum {
	UNKNOWN_CONTEXT,
	ARRAY_NAME,
	BARRIER,
	COMMON_NAME,
	CRITICAL,
	FILENAME,
	FLUSH,
	FUNCTION,
	FUNC_CALL,
	MASTER,
	OMPLOOP,
	ORDERED,
	PARALLEL,
	SECTIONS,
	SECTION_EVENT,
	SEQLOOP,
	SINGLE,
	THREADPRIVATE,
	VARIABLE_NAME,
	WORKSHARE,
} context_type;

typedef enum {
	UNKNOWN_REDOP,
	AND,
	EQV,
	IAND,
	IEOR,
	IOR,
	MAX,
	MIN,
	MINUS,
	NEQV,
	OR,
	PLUS,
	PRODUCT,
} redop_type;

typedef enum {
	UNKNOWN_BEHAVIOR,
	NONE,
	PRIVATE,
	SHARED,
} behavior_type;

typedef enum {
	UNKNOWN_SCHEDULING,
	DYNAMIC,
	GUIDED,
	RUNTIME,
	STATIC,
} schedule_type;

typedef enum {
	UNKNOWN_RT_TYPE,
	RT_CHAR,
	RT_DOUBLE,
	RT_DOUBLE_COMPLEX,
	RT_FLOAT,
	RT_FLOAT_COMPLEX,
	RT_INT,
	RT_LONG,
} variable_rt_type;

typedef struct {
	int count; // количество имен
	char **names; //список имен
} names_list;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
	names_list *private;
	names_list *shared;
	names_list *firstprivate;
	names_list *copyin;
	names_list *reduction;
	redop_type redop;
	behavior_type default_behavior;
	char *if_text;
	char *num_threads;
} parallel_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
	int is_ordered;
	int is_nowait;
	names_list *private;
	names_list *firstprivate;
	names_list *lastprivate;
	names_list *reduction;
	redop_type redop;
	schedule_type schedule;
	char *chunk_size;
} omploop_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
	int is_nowait;
	names_list *private;
	names_list *firstprivate;
	names_list *lastprivate;
	names_list *reduction;
	redop_type redop;
} sections_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
} section_event_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
	int is_nowait;
	names_list *private;
	names_list *firstprivate;
	names_list *copyprivate;
} single_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
	int is_nowait;
} workshare_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
} master_context_descriptor;

typedef struct {
	char *filename;
	char *name;
	int begin_line;
	int end_line;
} critical_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
} barrier_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
	names_list *flushed;
} flush_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
} ordered_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
	names_list *threadprivate;
} threadprivate_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
	char *var_name;
	variable_rt_type variable_type;
	int is_indata;
	int is_incommon;
	int is_insave;
} variable_name_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
	char *arr_name;
	variable_rt_type variable_type;
	int rank;
	int is_indata;
	int is_incommon;
	int is_insave;
} array_name_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
	char *block_name;
	names_list *components;
} common_name_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
} file_name_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
} seqloop_context_descriptor;

typedef struct {
	char *filename;
	int line_number;
	char *name;
	int args_count;
} func_call_context_descriptor;

typedef struct {
	char *filename;
	int begin_line;
	int end_line;
	char *func_name;
	int args_count;
} function_context_descriptor;

typedef struct {
	context_type type;
	void *context_ptr;
} context_descriptor;

context_type get_context_string_type(const char *context_string);
redop_type get_redop_type_from_name(const char *redop_name);
behavior_type get_behavior_type_from_name(const char *behavior_name);
schedule_type get_schedule_type_from_name(const char *schedule_name);
variable_rt_type get_variable_rt_type_from_name(const char *rt_type_name);

#endif
