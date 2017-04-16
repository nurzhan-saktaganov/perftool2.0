#ifndef LIST_H
#define LIST_H

typedef struct _node {
	struct _node *prev;
	struct _node *next;
	void *data;
} node;

typedef struct _list{
	node *head;
	node *tail;
	int size;
} list;

typedef struct _list_iterator {
	node *current;
} list_iterator;

list *list_create();
void list_destroy(list *l_p, void (*destroy_data)(void *));
void list_append_head(list *l_p, void *data);
void *list_remove_head(list *l_p);
void list_append_tail(list *l_p, void *data);
void *list_remove_tail(list *l_p);
void *list_peek_head(list *l_p);
void *list_peek_tail(list *l_p);
int list_size(list *l_p);
int list_has_element(list *l, void *data);

list_iterator *list_iterator_new(list *l_p);
int list_iterator_has_next(list_iterator *li_p);
void *list_iterator_next(list_iterator *li_p);
void list_iterator_destroy(list_iterator *li_p);

#endif
