#ifndef CONTEXT_TYPES_H
#define CONTEXT_TYPES_H

// CTN - CONTEXT_TYPE_NAME
#define CTN_ARRAY_NAME 		"arr_name"
#define CTN_BARRIER 		"barrier"
#define CTN_COMMON_NAME		"common_name"
#define CTN_CRITICAL		"critical"
#define CTN_FILE_NAME		"file_name"
#define CTN_FLUSH			"flush"
#define CTN_FUNCTION		"function"
#define CTN_FUNC_CALL		"func_call"
#define CTN_MASTER			"master"
#define CTN_OMPLOOP			"omploop"
#define CTN_ORDERED			"ordered"
#define CTN_PARALLEL		"parallel"
#define CTN_SECTIONS		"sections"
#define CTN_SECTION_EVENT	"sect_ev"
#define CTN_SEQLOOP			"seqloop"
#define CTN_SINGLE			"single"
#define CTN_THREADPRIVATE	"threadprivate"
#define CTN_VARIABLE_NAME	"var_name"
#define CTN_WORKSHARE		"workshare"

// RTN - REDOP_TYPE_NAME
#define RTN_PLUS		"PLUS"
#define RTN_MINUS		"MINUS"
#define RTN_PRODUCT		"PRODUCT"
#define RTN_AND			"AND"
#define RTN_OR			"OR"
#define RTN_EQV			"EQV"
#define RTN_NEQV		"NEQV"
#define RTN_MAX			"MAX"
#define RTN_MIN			"MIN"
#define RTN_IAND		"IAND"
#define RTN_IOR			"IOR"
#define RTN_IEOR		"IEOR"

// BTN - BEHAVIOR_TYPE_NAME
#define BTN_PRIVATE		"PRIVATE"
#define BTN_SHARED		"SHARED"
#define BTN_NONE		"NONE"

// STN - SHEDULE_TYPE_NAME
#define STN_STATIC		"STATIC"
#define STN_DYNAMIC		"DYNAMIC"
#define STN_GUIDED		"GUIDED"
#define STN_RUNTIME		"RUNTIME"

// VTN - VARIABLE_TYPE_NAME
#define VTN_CHAR				"0"
#define VTN_INT					"1"
#define VTN_LONG				"2"
#define VTN_FLOAT				"3"
#define VTN_DOUBLE				"4"
#define VTN_FLOAT_COMPLEX		"5"
#define VTN_DOUBLE_COMPLEX		"6"

typedef enum {
	CONTEXT_UNKNOWN,
	CONTEXT_ARRAY_NAME,
	CONTEXT_BARRIER,
	CONTEXT_COMMON_NAME,
	CONTEXT_CRITICAL,
	CONTEXT_FILE_NAME,
	CONTEXT_FLUSH,
	CONTEXT_FUNCTION,
	CONTEXT_FUNC_CALL,
	CONTEXT_MASTER,
	CONTEXT_OMPLOOP,
	CONTEXT_ORDERED,
	CONTEXT_PARALLEL,
	CONTEXT_SECTIONS,
	CONTEXT_SECTION_EVENT,
	CONTEXT_SEQLOOP,
	CONTEXT_SINGLE,
	CONTEXT_THREADPRIVATE,
	CONTEXT_VARIABLE_NAME,
	CONTEXT_WORKSHARE,
} context_type;

typedef enum {
	REDOP_UNKNOWN,
	REDOP_AND,
	REDOP_EQV,
	REDOP_IAND,
	REDOP_IEOR,
	REDOP_IOR,
	REDOP_MAX,
	REDOP_MIN,
	REDOP_MINUS,
	REDOP_NEQV,
	REDOP_OR,
	REDOP_PLUS,
	REDOP_PRODUCT,
} redop_type;

typedef enum {
	BEHAVIOR_UNKNOWN,
	BEHAVIOR_NONE,
	BEHAVIOR_PRIVATE,
	BEHAVIOR_SHARED,
} behavior_type;

typedef enum {
	SCHEDULING_UNKNOWN,
	SCHEDULING_DYNAMIC,
	SCHEDULING_GUIDED,
	SCHEDULING_RUNTIME,
	SCHEDULING_STATIC,
} schedule_type;

typedef enum {
	RT_UNKNOWN,
	RT_CHAR,
	RT_DOUBLE,
	RT_DOUBLE_COMPLEX,
	RT_FLOAT,
	RT_FLOAT_COMPLEX,
	RT_INT,
	RT_LONG,
} variable_rt_type;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
	list *names_private;
	list *names_shared;
	list *names_firstprivate;
	list *names_copyin;
	list *names_reduction;
	redop_type redop;
	behavior_type default_behavior;
	char *if_text;
	char *num_threads;
} parallel_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
	int is_ordered;
	int is_nowait;
	list *names_private;
	list *names_firstprivate;
	list *names_lastprivate;
	list *names_reduction;
	redop_type redop;
	schedule_type schedule;
	char *chunk_size;
} omploop_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
	int is_nowait;
	list *names_private;
	list *names_firstprivate;
	list *names_lastprivate;
	list *names_reduction;
	redop_type redop;
} sections_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
} section_event_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
	int is_nowait;
	list *names_private;
	list *names_firstprivate;
	list *names_copyprivate;
} single_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
	int is_nowait;
} workshare_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
} master_context_descriptor;

typedef struct {
	char *file_name;
	char *critical_name;
	int begin_line;
	int end_line;
} critical_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
} barrier_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
	list *names_flushed;
} flush_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
} ordered_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
	list *names_threadprivate;
} threadprivate_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
	char *var_name;
	variable_rt_type variable_type;
	int is_indata;
	int is_incommon;
	int is_insave;
} variable_name_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
	char *arr_name;
	variable_rt_type variable_type;
	int rank;
	int is_indata;
	int is_incommon;
	int is_insave;
} array_name_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
	char *block_name;
	list *names_components;
} common_name_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
} file_name_context_descriptor;

typedef struct {
	char *file_name;
	int begin_line;
	int end_line;
} seqloop_context_descriptor;

typedef struct {
	char *file_name;
	int line_number;
	char *func_name;
	int args_count;
} func_call_context_descriptor;

typedef struct {
	char *file_name;
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
