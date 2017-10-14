#ifndef STACK_H
#define STACK_H

#include "list.h"

// Стек
typedef list stack;

// Возвращает новый стек
stack *stack_create();

// Уничтожает стек
void stack_destroy(stack *s);

// Кладет элемент в стек
void stack_push(stack *s, void *data);

// Извлекает элемент из стека
void *stack_pop(stack *s);

// Возвращает верхний элемент стека
void *stack_peek(stack *s);

// Возвращает размер стека
int stack_size(stack *s);

#endif
