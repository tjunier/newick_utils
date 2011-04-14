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

/*
 * NOTE: Care should be exercised when manipulating the lists (e.g.
 * appending a list to another list), lest it become very hairy to free() them
 * without free()ing something twice.  shallow_copy() is your friend. */

/* NOTE: functions that return pointers will return NULL in case of error (most
 * probably a malloc() problem due to insufficient memory. Functions that just
 * perform an action (i.e., who could return void) return SUCCESS or FAILURE.
 * FAILURE will often be caused by insufficient memory in a called function. 
 */

/* Note: we could also make these two structures private and use only
 * accessors. But they are so pervasive that it would entail many additional
 * function calls, which may impact performance. Check before deciding. */

/** An element of a linked list (see struct llist). */

struct list_elem {
	struct list_elem *next;	/**< next element (NULL at end of list) */
	void *data;		/**< arbitrary data */
};

/** A linked list. Apart from the head pointer, it also maintains a pointer to
 * the tail, to enable finding the tail in constant time (useful for
 * appending); and a count of elements (for finding teh length in constant
 * time). */

struct llist {
	struct list_elem *head;	/**< first element, NULL in an empty list. */
	/** last element, equal to head if list has 0 or 1 element (NULL in the
	 * first case) */
	struct list_elem *tail;
	int count;		/**< number of elements in list (0: empty) */
};

/* allocates a llist and returns a pointer to it.
   Sets head to NULL and count to 0 */

struct llist *create_llist();

/* adds data to the beginning of the list (becomes
   new head). */

int prepend_element(struct llist *list, void *data);

/* adds data to end of list */

int append_element(struct llist *list, void *data);

/* Appends list to target list, by redirecting target's pointers and updating
 * count - no new memory allocation . The list structure pointed to by 'insert'
 * is not obsolete, but it points to the middle of a list, which may or may not
 * be desired.*/

void append_list(struct llist *target, struct llist *insert);

/* prepends list to target list  - see comments for append_list(). */

void prepend_list(struct llist *target, struct llist *insert);

/* returns a new list that is the reverse of the argument */

struct llist *llist_reverse(struct llist *list);

/* Returns a copy of the list (the data themselves are not cloned) */

struct llist *shallow_copy(struct llist *list);

/* Shifts the first element (head) off the list, and returns its data. The
 * list_elem structure is freed. */

void *shift(struct llist *);

/* Reduces list to a single element by popping the first two, and
 * replacing them by the result of (*func)() applied to them, until there is
 * only one element left.  (*func)() should return NULL iff there is an error.
 * That way reduce() will return NULL iff there is an error. */ 

void *reduce(struct llist *, void* (*func)(void *, void *));

/* Dumps the contents of 'list' to stdout. Elements are dumped via function
 * 'dump_func()', which must be supplied. */

void dump_llist(struct llist *list, void (*dump_func)(void *));

/* Inserts 'insert' into 'target' after position 'pos'. List elements are
 * numbered from the list's head, which has index 0. Set 'pos' to -1 to prepend
 * to list, and to target->count -1 to append. Positions outside this range are
 * ignored and nothing is done. NOTE: This function changes 'target', so be
 * careful if you use it afterwards (you may want to do a shallow_copy()
 * first). */

void insert_after(struct llist *target, int pos, struct llist *insert);

/* Removes 'length' elements from 'target', starting at 'pos' + 1. Returns a
 * list made from the removed elements. */

struct llist *delete_after(struct llist *target, int pos, int length);

/* Returns the index of the 1st element whose data is 'data'. This does not
 * compare data, just their addresses (see llist_index_of_f() for that).
 * Returns -1 if data is not in list. */

int llist_index_of(struct llist *list, void *data);

/* Looks for target 'tgt' in a list. Applies function 'func()' to the elements
 * of the list, until 'func()' returns true. The list element's data is passed
 * as the first argument, and the second argument is always 'tgt'. One use of
 * this is to look for strings, which cannot be done with llist_index_of()
 * because it only compares addresses (whereas this function calls an arbitrary
 * function) */

int llist_index_of_f(struct llist *list, int (*func)(void*,void*), void *tgt);

/* Destroys list and releases memory. List data are NOT destroyed or released,
 * only the data structure (i.e., the llist itself and the list_elements). To
 * delete all elements without deleting the llist itself, use clear_llist(). */

void destroy_llist(struct llist *);

/* Returns an array of pointers to all elements in the list, in the list order.
 * */

void ** llist_to_array(struct llist *);

/* Returns a list based on an array of pointers (which will be attributed to
 * the data member of the list elements). Second argument is the number of
 * elements in the array. */

struct llist * array_to_llist(void **, int count);

/* Removes all elements (and frees them). Element data is not freed(). List
 * structure itself is retained (i.e., pointer to struct llist is still valid)
 * - to destroy list altogether, use destroy_llist(). */

void clear_llist(struct llist *);
