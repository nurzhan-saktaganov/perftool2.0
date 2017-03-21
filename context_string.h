#ifndef CONTEXT_STRING_H
#define CONTEXT_STRING_H


typedef struct {
	int count; // количество имен
	char **names; //список имен
} names_list;

char *get_param_str_value(const char *context_string, const char *param_name);
int get_param_int_value(const char *context_string, const char *param_name);
names_list *get_param_names_list(const char *context_string, const char *param_name);

#endif
