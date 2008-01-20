
struct list_elem {
	struct list_elem *next;
	void *data;
};

struct llist {
	struct list_elem *head;
	struct list_elem *tail;
	int count;
};

/* allocates a llist and returns a pointer to it.
   Sets head to NULL and count to 0 */

struct llist *create_llist();

/* adds data to the beginning of the list (becomes
   new head). */

void prepend_element(struct llist *list, void *data);

/* adds data to end of list */

void append_element(struct llist *list, void *data);

/* returns a new list that is the reverse of the argument */

struct llist *llist_reverse(struct llist *list);

/* Returns a copy of the list (the data themselves are not cloned) */

struct llist *shallow_copy(struct llist *list);

/* Pops the last element (tail) off the list, and returns it */

void *shift(struct llist *);

/* Reduces list to a single element by popping the first two, and
 * replacing them by the result of (*func)() applied to them */ 

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

/* Removes 'length' elements from 'target', starting at 'pos'. Returns a list
 * made from th eremoved elements. */

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
 * only the data structure (i.e., the llist itself and the list_elements). */

void destroy_llist(struct llist *);
