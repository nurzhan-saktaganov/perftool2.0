#ifndef CONTEXT_STRING_H
#define CONTEXT_STRING_H

#include <list.h>

// Возвращает строкое значение параметра контекстной строки
char *get_param_str_value(const char *context_string, const char *param_name);

// Возвращает числовое значение параметра контекстной строки
int get_param_int_value(const char *context_string, const char *param_name);

// Возвращает список строковых значений параметра контекстной строки
list *get_param_names_list(const char *context_string, const char *param_name);

#endif
