/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "list.h"


struct llist *create_llist()
{
	struct llist *llist_p;
	llist_p = malloc(sizeof(struct llist));
	if (NULL == llist_p) return NULL;
	llist_p->head = llist_p->tail = NULL;
	llist_p->count = 0;
	return llist_p;
}

int prepend_element(struct llist *list, void *data)
{
	struct list_elem *el_p;

	el_p = malloc(sizeof(struct list_elem));
	if (NULL == el_p) return FAILURE;
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

	return SUCCESS;
}

int append_element(struct llist *list, void *data)
{
	struct list_elem *el_p;

	el_p = malloc(sizeof(struct list_elem));
	if (NULL == el_p) return FAILURE;
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

	return SUCCESS;
}

struct llist *llist_reverse(struct llist *list)
{
	struct llist *result;
	struct list_elem *elem;

	result = create_llist();
	if (NULL == result) return NULL;

	for (elem = list->head; NULL != elem; elem = elem->next) 
		if (! prepend_element(result, elem->data))
			return NULL;

	return result;
}

struct llist *shallow_copy(struct llist *orig)
{
	struct llist *copy = create_llist();
	if (NULL == copy) return NULL;
	struct list_elem *elem;

	for (elem = orig->head; NULL != elem; elem = elem->next) {
		if (! append_element(copy, elem->data))
			return NULL;
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
		if (NULL == func_result) return NULL;
		if (! prepend_element(list, func_result)) return NULL;
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

	/* special cases */
	if (0 == insert->count) return;

	if (0 == target->count) {
		target->head = insert->head;
		target->tail = insert->tail;
		target->count = insert->count;
		return;
	}

	struct list_elem *old_head;
	old_head = target->head;
	target->head = insert->head;
	insert->tail->next = old_head;

	target->count += insert->count;
}

void append_list(struct llist *target, struct llist *insert)
{
	/* special cases */
	if (0 == insert->count) return;

	if (0 == target->count) {
		target->head = insert->head;
		target->tail = insert->tail;
		target->count = insert->count;
		return;
	}

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
	if (NULL == result) return NULL;

	if (-1 == pos) {
		elem = target->head;
		result->head = target->head;
		for (n = length;  n > 1; n--) /* not 0 */
			elem = elem->next;
		result->tail = elem;
		elem = elem->next;	/* new head of target */
		target->head = elem;
		if (result->tail == target->tail)
			target->tail = NULL;
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
	if (NULL == array) return NULL;

	struct list_elem *el;
	int i;
	for (i = 0, el = l->head; NULL != el; i++, el = el->next)
		array[i] = el->data;

	return array;
}

struct llist *array_to_llist(void **array, int count)
{
	struct llist *list = create_llist();
	if (NULL == list) return NULL;
	int i;

	for (i = 0; i < count; i++) 
		if (! append_element(list, array[i]))
			return NULL;

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
