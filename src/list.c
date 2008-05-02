#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "list.h"

struct llist *create_llist()
{
	struct llist *llist_p;
	llist_p = malloc(sizeof(struct llist));
	if (NULL == llist_p) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	llist_p->head = llist_p->tail = NULL;
	llist_p->count = 0;
	return llist_p;
}

void prepend_element(struct llist *list, void *data)
{
	struct list_elem *el_p;

	el_p = malloc(sizeof(struct list_elem));
	if (NULL == el_p) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	el_p->data = data;
	el_p->next = NULL;

	if (NULL == list->head)  {
		list->head = el_p;
		list->tail = el_p;
	} else {
		el_p->next = list->head;
		list->head = el_p;
	}
	list->count += 1;
}

void append_element(struct llist *list, void *data)
{
	struct list_elem *el_p;

	el_p = malloc(sizeof(struct list_elem));
	if (NULL == el_p) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}
	el_p->data = data;
	el_p->next = NULL;

	if (NULL == list->head) {
		list->head = el_p;
		list->tail = el_p;
	} else {
		list->tail->next = el_p;
		list->tail = el_p;
	}
	list->count += 1;
}

struct llist *llist_reverse(struct llist *list)
{
	struct llist *result;
	struct list_elem *elem;

	result = create_llist();

	for (elem = list->head; NULL != elem; elem = elem->next) 
		prepend_element(result, elem->data);

	return result;
}

struct llist *shallow_copy(struct llist *orig)
{
	struct llist *copy = create_llist();
	struct list_elem *elem;

	for (elem = orig->head; NULL != elem; elem = elem->next) {
		append_element(copy, elem->data);
	}

	return copy;
}

void *shift(struct llist *list)
{
	struct list_elem *result;

	assert(list->count > 0);
	assert(NULL != list->head);

	result = list->head;
	list->head = list->head->next;
	list->count -= 1;
	
	void *data = result->data;
	free(result);

	return data;
}

void *reduce(struct llist *list, void* (*func)(void *, void*))
{
	void *func_result;
	void *top, *second;

	while (list->count > 1) {
		assert(NULL != list->head);
		top = shift(list);
		assert(NULL != list->head);
		second = shift(list);
		func_result = func(top, second);
		prepend_element(list, func_result);
	}

	return shift(list);
}

void dump_llist(struct llist *list, void (*dump_func)(void *))
{
	struct list_elem *el;

	printf ("Dump of list at %p (length %d):\n", list, list->count);
	for (el = list->head; NULL != el; el = el->next)
		dump_func(el->data);
}

void prepend_list(struct llist *target, struct llist *insert)
{
	struct list_elem *old_head;

	old_head = target->head;
	target->head = insert->head;
	insert->tail->next = old_head;

	target->count += insert->count;
}

void append_list(struct llist *target, struct llist *insert)
{
	target->tail->next = insert->head;
	target->tail = insert->tail;

	target->count += insert->count;
}

void insert_after(struct llist *target, int pos, struct llist *insert)
{
	struct list_elem *elem;
	struct list_elem *after_insert;

	if (-1 == pos) {
		/* special case: prepend */
		prepend_list(target, insert);
		return ;
	} else if ((pos >= 0) && (pos <= target->count-2)) {
		/* general case */
		for (elem = target->head; pos > 0; pos--)
			elem = elem->next;
		after_insert = elem->next;
		elem->next = insert->head;
		insert->tail->next = after_insert;
		target->count += insert->count;
		return ;
	} else if (pos == target->count -1) {
		/* special case: append */
		append_list(target, insert);
		return ;
	} else {
		/* unexpected index - this is not too bad, but it does indicate
		 * wrong parameters */
		/* We do nothing in this case - caller should check arguments. */
		return ;
	}
}

struct llist *delete_after(struct llist *target, int pos, int length)
{
	struct llist *result;
	struct list_elem *elem;
	int i, n;

	result = create_llist();

	if (-1 == pos) {
		elem = target->head;
		result->head = target->head;
		for (n = length;  n > 1; n--) /* not 0 */
			elem = elem->next;
		result->tail = elem;
		elem = elem->next;	/* new head of target */
		target->head = elem;
		result->tail->next = NULL;
		/* target's tail does not change */
	} else if ((pos >= 0) && (pos < target->count - length -1)) {
		struct list_elem *last_before_del;
		elem = target->head;
		for (i = pos; i > 0; i--)
			elem = elem->next;
		last_before_del = elem;
		elem = elem->next;
		result->head = elem;
		for (n = length; n > 1; n--) /* not 0 */
			elem = elem->next;
		result->tail = elem;
		elem = elem->next;
		last_before_del->next = elem;
		result->tail->next = NULL;
	} else if (pos + length + 1 == target->count) {
		elem = target->head;
		result->tail = target->tail;
		for (i = pos; i > 0; i--) /* not 0 */
			elem = elem->next;
		target->tail = elem;
		elem = elem->next;
		target->tail->next = NULL;
		result->head = elem;
	}

	target->count -= length;
	result->count = length;

	return result;
}

int llist_index_of(struct llist *list, void *data)
{
	struct list_elem *elem;
	int n;

	for (n = 0, elem = list->head; NULL != elem; n++,  elem = elem->next) 
		if (data == elem->data)
			return n;

	return -1;
}
		
int llist_index_of_f(struct llist *list, int (*func)(void*,void*), void * tgt)
{
	struct list_elem *el;
	int n;

	for (n = 0, el = list->head; NULL != el; n++, el = el->next) {
		if (func(el->data, tgt))
			return n;
	}
	return -1;
}

void destroy_llist(struct llist *l) 
{
	clear_llist(l);
	free(l);
}

void ** llist_to_array (struct llist *l)
{
	if (0 == l->count) { return NULL; }

	void ** array = malloc(l->count * sizeof(void *));
	if (NULL == array) { perror(NULL); exit(EXIT_FAILURE); }

	struct list_elem *el;
	int i;
	for (i = 0, el = l->head; NULL != el; i++, el = el->next)
		array[i] = el->data;

	return array;
}

struct llist *array_to_llist(void **array, int count)
{
	struct llist *list = create_llist();
	int i;

	for (i = 0; i < count; i++) 
		append_element(list, array[i]);

	return list;
}

void clear_llist(struct llist *l)
{
	struct list_elem *e, *f;
	e = l->head;
	while (NULL != e) {
		f = e->next;
		free(e);
		e = f;
	}
	l->head = l->tail = NULL;
	l->count = 0;
}
