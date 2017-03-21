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


list *list_create();
void list_delete(list **l_pp, void (*delete_data)(void *));
void list_append_head(list *l_p, void *data);
void *list_remove_head(list *l_p);
void list_append_tail(list *l_p, void *data);
void *list_remove_tail(list *l_p);
void *list_peek_head(list *l_p);
void *list_peek_tail(list *l_p);
int list_size(list *l_p);

#endif
