#include <assert.h>
#include <stdlib.h>
#include "list.h"

typedef struct _node {
	struct _node *_prev;
	struct _node *_next;
	void *_data;
} node;

struct _list {
	node *_head;
	node *_tail;
	int _size;
};

struct _list_iterator {
	node *_current;
};

list *list_create()
{
	list *l = (list *) malloc(sizeof(list));
	assert(l);
	l->_head = l->_tail = NULL;
	l->_size = 0;
	return l;
}

void list_destroy(list *l)
{
	list_destroy_with(l, NULL);
}

void list_destroy_with(list *l, list_element_destroy_t f)
{
	assert(l);
	node *current = l->_head;
	while (current){
		if (f) {
			f(current->_data);
		}
		node* next = current->_next;
		free(current);
		current = next;
	}
	free(l);
}

void list_append_head(list *l, void *data)
{
	assert(l);
	node *new_node = (node *) malloc(sizeof(node));
	assert(new_node);

	new_node->_data = data;

	new_node->_prev = NULL;
	new_node->_next = l->_head;

	if (l->_head){
		l->_head->_prev = new_node;
	} else {
		l->_tail = new_node;
	}

	l->_head = new_node;
	l->_size += 1;
}

void *list_remove_head(list *l)
{
	assert(list_size(l) > 0);

	node *head = l->_head;
	void *data = head->_data;

	l->_head = l->_head->_next;

	if (l->_head){
		l->_head->_prev = NULL;
	} else {
		l->_tail = NULL;
	}

	l->_size -= 1;
	free(head);

	return data;
}

void list_append_tail(list *l, void *data)
{
	assert(l);
	node *new_node = (node *) malloc(sizeof(node));
	assert(new_node);

	new_node->_data = data;

	new_node->_prev = l->_tail;
	new_node->_next = NULL;

	if (l->_tail){
		l->_tail->_next = new_node;
	} else {
		l->_head = new_node;
	}

	l->_tail = new_node;
	l->_size += 1;
}

void *list_remove_tail(list *l)
{
	assert(list_size(l) > 0);

	node *tail = l->_tail;
	void *data = tail->_data;

	l->_tail = l->_tail->_prev;

	if (l->_tail){
		l->_tail->_next = NULL;
	} else {
		l->_head = NULL;
	}

	l->_size -= 1;
	free(tail);

	return data;
}

void *list_peek_head(list *l)
{
	assert(list_size(l) > 0);
	return l->_head->_data;
}

void *list_peek_tail(list *l)
{
	assert(list_size(l) > 0);
	return l->_tail->_data;
}

int list_size(list *l)
{
	assert(l);
	return l->_size;
}

int list_has_element(list *l, void *data)
{
	assert(l);
	node *current = l->_head;
	while (current){
		if (current->_data == data){
			return 1;
		}
		current = current->_next;
	}
	return 0;
}

list_iterator *list_iterator_new(list *l)
{
	assert(l);
	list_iterator *i = (list_iterator *) malloc(sizeof(list_iterator));
	assert(i);
	i->_current = l->_head;
	return i;
}

int list_iterator_has_next(list_iterator *i)
{
	assert(i);
	return i->_current != NULL;
}

void *list_iterator_next(list_iterator *i)
{
	assert(list_iterator_has_next(i));
	void *data = i->_current->_data;
	i->_current = i->_current->_next;
	return data;
}

void list_iterator_destroy(list_iterator *i)
{
	assert(i);
	free(i);
}
