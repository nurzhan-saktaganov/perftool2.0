#include "list.h"
#include "stack.h"

stack *stack_create()
{
	return list_create();
}

void stack_destroy(stack *s)
{
	list_destroy(s);
}

void stack_push(stack *s, void *data)
{
	list_append_tail(s, data);
}

void *stack_pop(stack *s)
{
	return list_remove_tail(s);
}

void *stack_peek(stack *s)
{
	return list_peek_tail(s);
}

int stack_size(stack *s)
{
	return list_size(s);
}
