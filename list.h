#ifndef LIST_H
#define LIST_H

// TODO Удалить весь неиспользуемый функционал

// Двусвязный список
typedef struct _list list;

// Итератор для списка
typedef struct _list_iterator list_iterator;

// Возвращает новый список
list *list_create();

// Уничтожает список
void list_destroy(list *l);

// Добавляет элемент в начало списка
void list_append_head(list *l, void *data);

// Удаляет и возвращает первый элемент списка
void *list_remove_head(list *l);

// Добавляет элемент в конец списка
void list_append_tail(list *l, void *data);

// Удаляет и возвращает последний элемента списка
void *list_remove_tail(list *l);

// Возвращает первый элемент списка
void *list_peek_head(list *l);

// Возвращает последний элемент списка
void *list_peek_tail(list *l);

// Возвращает размер списка
int list_size(list *l);

// Проверяет наличие элемента в списке
int list_has_element(list *l, void *data);

// Возвращает новый итератор
list_iterator *list_iterator_new(list *l);

// Проверяет есть ли еще элементы в итераторе
int list_iterator_has_next(list_iterator *i);

// Возвращает следующий элемент итератора
void *list_iterator_next(list_iterator *l);

// Уничтожает итератор
void list_iterator_destroy(list_iterator *li_p);

#endif
