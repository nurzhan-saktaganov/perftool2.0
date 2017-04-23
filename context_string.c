#include <assert.h>
#include <string.h>
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
	int param_name_len;
	int context_str_len;
	char *_param_name;
	char *found_at;
	char *str_value;
	char *begin, *end;
	int elements_to_copy;
	int copied;
	
	if (context_string == NULL || param_name == NULL) {
		return NULL;
	}
	// minimal context string -> "2**";
	context_str_len = strlen(context_string);
	if (context_str_len < 3 || context_string[context_str_len - 1] != '*'){
		return NULL;
	}
	
	// проверяем есть ли параметр с таким именем
	param_name_len = strlen(param_name);
	_param_name = (char *) malloc((param_name_len + 3) * sizeof(char)); //for '*','=','\0'
	assert(_param_name);
	sprintf(_param_name, "*%s=", param_name);
	found_at = strstr(context_string, _param_name);
	free(_param_name);
	if (found_at == NULL) {
		return NULL;
	}
	
	/* Этот кусочек кода для случая, когда "\*" => "*": НАЧАЛО */
	// считаем сколько элементов нужно скопировать
	elements_to_copy = 0;
	begin = end = found_at + param_name_len + 2; // первый знак за знаком равно
	while (!(*end == '*' && end[-1] != '\\')){
		if (*end != '*') {
			++elements_to_copy;
		}
		++end;
	}
	
	str_value = (char *) malloc((elements_to_copy + 1) * sizeof(char));
	assert(str_value);
	
	// копируем с приведением "\*" -> "*"
	copied = 0;
	while (begin < end){
		if (*begin == '*') {
			copied--;
		}
		str_value[copied++] = *begin++;
	}
	str_value[copied] = '\0';
	/* Этот кусочек кода для случая, когда "\*" => "*": КОНЕЦ */
	
	
	/* Этот кусочек кода для случая, когда "*\" => "*": НАЧАЛО *//*
	// считаем сколько элементов нужно скопировать
	elements_to_copy = 0;
	begin = end = found_at + param_name_len + 2; // первый знак за знаком равно
	while (!(*end == '*' && end[1] != '\\')) {
		if (*end != '*'){
			++elements_to_copy;
		}
		++end;
	}
	
	str_value = (char *) malloc((elements_to_copy + 1) * sizeof(char));
	assert(str_value);
	
	// копируем с приведением "*\" -> "*"
	copied = 0;
	while (begin < end){
		str_value[copied++] = *begin++;
		if (begin[-1] == '*'){
			++begin;
		}
	}
	str_value[copied] = '\0';
	*//* Этот кусочек кода для случая, когда "*\" => "*": КОНЕЦ */
	return str_value;
}


int get_param_int_value(const char *context_string, const char *param_name)
{
	char *str_value;
	int int_value;
	str_value = get_param_str_value(context_string, param_name);
	if (str_value == NULL){
		return 0; // errno?
	}
	int_value = atoi(str_value);
	free(str_value);
	return int_value;
}

list *get_names_list(const char *comma_separated_names)
{
	list *names;
	char *token, *save_ptr;
	char *_comma_separated_names;
	
	if (comma_separated_names == NULL){
		return NULL;
	}
	
	_comma_separated_names = strdup(comma_separated_names);
	
	token = strtok_r(_comma_separated_names, ",", &save_ptr);
	if (token == NULL){
		free(_comma_separated_names);
		return NULL;
	}
	names = list_create();
	while (token != NULL) {
		list_append_tail(names, strdup(token));
		token = strtok_r(NULL, ",", &save_ptr);
	}
	free(_comma_separated_names);
	return names;
}


list *get_param_names_list(const char *context_string, const char *param_name)
{
	list *names;
	char *str_value;
	str_value = get_param_str_value(context_string, param_name);
	if (str_value == NULL){
		return NULL;
	}
	names = get_names_list(str_value);
	free(str_value);
	return names;
}
