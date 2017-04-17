#include <stdlib.h>
#include "list.h"

typedef struct _node {
	struct _node *prev;
	struct _node *next;
	void *data;
} node;

struct _list {
	node *head;
	node *tail;
	int size;
};

struct _list_iterator {
	node *current;
};

list *list_create()
{
	list *l_p = (list *) malloc(sizeof(list));
	l_p->head = NULL;
	l_p->tail = NULL;
	l_p->size = 0;
	return l_p;
}

void list_destroy(list *l_p, void (*destroy_data)(void *))
{
	if (l_p == NULL){
		return;
	}
	node *current, *next;
	current = l_p->head;
	while (current != NULL){
		if (destroy_data != NULL){
			destroy_data(current->data);
		}
		next = current->next;
		free(current);
		current = next;
	}
	free(l_p);
	return;
}

void list_append_head(list *l_p, void *data)
{
	node *new_node = (node *) malloc(sizeof(node));
	new_node->data = data;
	new_node->prev = NULL;
	new_node->next = l_p->head;
	if (l_p->head != NULL){
		l_p->head->prev = new_node;
	} else {
		l_p->tail = new_node;
	}
	l_p->head = new_node;
	l_p->size += 1;
	return;
}

void *list_remove_head(list *l_p)
{
	if (l_p == NULL || l_p->head == NULL){
		return NULL;
	}
	node *first = l_p->head;
	void *data = l_p->head->data;
	l_p->head = l_p->head->next;
	if (l_p->head != NULL){
		l_p->head->prev = NULL;
	} else {
		l_p->tail = NULL;
	}
	l_p->size -= 1;
	free(first);
	return data;
}

void list_append_tail(list *l_p, void *data)
{
	node *new_node = (node *) malloc(sizeof(node));
	new_node->data = data;
	new_node->prev = l_p->tail;
	new_node->next = NULL;
	if (l_p->tail != NULL){
		l_p->tail->next = new_node;
	} else {
		l_p->head = new_node;
	}
	l_p->tail = new_node;
	l_p->size += 1;
	return;
}

void *list_remove_tail(list *l_p)
{
	if (l_p == NULL || l_p->tail == NULL){
		return NULL;
	}
	node *last = l_p->tail;
	void *data = l_p->tail->data;
	l_p->tail = l_p->tail->prev;
	if (l_p->tail != NULL){
		l_p->tail->next = NULL;
	} else {
		l_p->head = NULL;
	}
	l_p->size -= 1;
	free(last);
	return data;
}

void *list_peek_head(list *l_p)
{
	if (l_p == NULL || l_p->head == NULL){
		return NULL;
	} 
	return l_p->head->data;
}

void *list_peek_tail(list *l_p)
{
	if (l_p == NULL || l_p->tail == NULL){
		return NULL;
	}
	return l_p->tail->data;
}

int list_size(list *l_p)
{
	return l_p != NULL ? l_p->size : 0;
}

int list_has_element(list *l, void *data)
{
	node *n = l->head;
	while (n != NULL){
		if (n->data == data){
			return 1;
		}
		n = n->next;
	}
	return 0;
}

list_iterator *list_iterator_new(list *l_p)
{
	if (l_p == NULL){
		return NULL;
	}
	list_iterator *li_p = (list_iterator *) malloc(sizeof(list_iterator));
	li_p->current = l_p->head;
	return li_p;
}

int list_iterator_has_next(list_iterator *li_p)
{
	if (li_p == NULL || li_p->current == NULL){
		return 0;
	}
	return 1;
}

void *list_iterator_next(list_iterator *li_p)
{
	void *data = li_p->current->data;
	li_p->current = li_p->current->next;
	return data;
}

void list_iterator_destroy(list_iterator *li_p)
{
	if (li_p == NULL){
		return;
	}
	free(li_p);
	return;
}
