#include <assert.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "context_string.h"

/* Ищет строковое значение параметра с именем param_name
 * в строке context_string.
 * Возвращает новую строку, если параметр с заданным именем найден.
 * Указатель на новую строку должен быть освобожден после использования.
 * Возвращает NULL, если ничего не найдено.
 */
char *get_param_str_value(const char *context_string, const char *param_name)
{
	assert(context_string && param_name);
	
	// minimal context string -> "2**";
	int context_str_len = strlen(context_string);
	assert(context_str_len > 2 && context_string[context_str_len-1] == '*');
	
	// проверяем есть ли параметр с таким именем
	int param_name_len = strlen(param_name);
	char *_param_name = (char *) malloc(param_name_len + 3); //for '*','=','\0'
	assert(_param_name);
	sprintf(_param_name, "*%s=", param_name);
	char *found_at = strstr(context_string, _param_name);
	free(_param_name);
	if (found_at == NULL) {
		return NULL;
	}
	
	// считаем сколько элементов нужно скопировать
	int elements_to_copy = 0;
	char *end = found_at + param_name_len + 2; // первый знак за знаком равно
	while (!(*end == '*' && end[-1] != '\\')){ // while not stop
		if (*end != '*') {
			++elements_to_copy;
		}
		++end;
	}
	
	char *str_value = (char *) malloc((elements_to_copy + 1) * sizeof(char));
	assert(str_value);
	
	// копируем с приведением "\*" -> "*"
	int copied = 0;
	char *begin = found_at + param_name_len + 2; // первый знак за знаком равно
	while (begin < end){
		if (*begin == '*') {
			--copied;
		}
		str_value[copied++] = *begin++;
	}
	str_value[copied] = '\0';
	return str_value;
}

int get_param_int_value(const char *context_string, const char *param_name)
{
	char *str_value = get_param_str_value(context_string, param_name);
	if (str_value == NULL){
		return 0;
	}
	int int_value = atoi(str_value);
	free(str_value);
	return int_value;
}

static list *get_names_list(const char *separated_names, const char *separator)
{
	assert(separated_names && separator);
	char *_separated_names = strdup(separated_names);
	assert(_separated_names);
	
	char *save_ptr;
	char *token = strtok_r(_separated_names, separator, &save_ptr);
	if (token == NULL){
		free(_separated_names);
		return NULL;
	}
	list *names = list_create();
	while (token != NULL) {
		list_append_tail(names, strdup(token));
		token = strtok_r(NULL, separator, &save_ptr);
	}
	free(_separated_names);
	return names;
}


list *get_param_names_list(const char *context_string, const char *param_name)
{
	assert(context_string && param_name);
	char *str_value = get_param_str_value(context_string, param_name);
	if (str_value == NULL){
		return NULL;
	}
	list *names = get_names_list(str_value, ",");
	free(str_value);
	return names;
}
