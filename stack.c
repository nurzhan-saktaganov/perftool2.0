#include "list.h"
#include "stack.h"

stack *stack_create()
{
	return list_create();
}

void stack_delete(list **s_p_p, void (*delete_data)(void *))
{
	list_delete(s_p_p, delete_data);
}

void stack_push(stack *s_p, void *data)
{
	list_append_head(s_p, data);
}

void *stack_pop(stack *s_p)
{
	return list_remove_head(s_p);
}

void *stack_peek(stack *s_p)
{
	return list_peek_head(s_p);
}

int stack_size(stack *s_p)
{
	return list_size(s_p);
}
