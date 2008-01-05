#include <check.h>

#include "../src/list.h"

struct test_data {
	float height;
	float length;
	char name[10];
};

START_TEST (test_create)
{
	struct llist *list_p;

	list_p = create_llist();

	fail_if (NULL == list_p, "list created NULL");
	fail_if (NULL != list_p->head,"list head not NULL");
	fail_if (NULL != list_p->tail,"list tail not NULL");
	fail_if (0 != list_p->count,"list count not 0");
}
END_TEST

START_TEST (test_prepend_1_element)
{
	struct llist *list_p;
	char *data = "blah";
	list_p = create_llist();
	prepend_element(list_p, data);
	fail_if(NULL == list_p->head, "head is NULL");
	fail_if(list_p->tail != list_p->head, "head != tail");
	fail_if(0 != strcmp((char *) list_p->head->data, data),
			"wrong data");
	fail_if(list_p->count != 1, "wrong count");
}
END_TEST

START_TEST (test_append_1_element)
{
	struct llist *list_p;
	char *data = "blah";
	list_p = create_llist();
	append_element(list_p, data);
	fail_if(NULL == list_p->head, "head is NULL");
	fail_if(list_p->tail != list_p->head, "head != tail");
	fail_if(0 != strcmp((char *) list_p->head->data, data),
			"wrong data");
	fail_if(list_p->count != 1, "wrong count");
}
END_TEST

START_TEST (test_prepend_five)
{
	char *test_name = "test_prepend_five";
	struct llist *list_p;
	struct list_elem *elem;

	list_p = create_llist();
	prepend_element(list_p, "one");
	prepend_element(list_p, "two");
	prepend_element(list_p, "three");
	prepend_element(list_p, "four");
	prepend_element(list_p, "five");
	/* note that adding to head reverses list */
	elem = list_p->head;
	fail_if (strcmp((char *) elem->data, "five"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "four"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "three"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "two"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "one"), "wrong data");
	fail_if (5 != list_p->count, "wrong count");
}
END_TEST

START_TEST (test_append_five)
{
	char *test_name = "test_append_five";
	struct llist *list_p;
	struct list_elem *elem;

	list_p = create_llist();
	append_element(list_p, "one");
	append_element(list_p, "two");
	append_element(list_p, "three");
	append_element(list_p, "four");
	append_element(list_p, "five");

	elem = list_p->head;
	if (strcmp(elem->data, "one") != 0) {
		printf ("%s: expected 'one', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "two") != 0) {
		printf ("%s: expected 'two', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "three") != 0) {
		printf ("%s: expected 'three', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "four") != 0) {
		printf ("%s: expected 'four', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "five") != 0) {
		printf ("%s: expected 'five', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	if(list_p->count != 5) {
		printf ("%s: count should be 5.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_reverse)
{
	char *test_name = "test_reverse";
	struct llist *list_p, *revlist_p;
	struct list_elem *elem;

	list_p = create_llist();
	append_element(list_p, "one");
	append_element(list_p, "two");
	append_element(list_p, "three");
	append_element(list_p, "four");
	append_element(list_p, "five");

	revlist_p = llist_reverse(list_p);

	elem = revlist_p->head;
	if (strcmp(elem->data, "five") != 0) {
		printf ("%s: expected 'five', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "four") != 0) {
		printf ("%s: expected 'four', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "three") != 0) {
		printf ("%s: expected 'three', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "two") != 0) {
		printf ("%s: expected 'two', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "one") != 0) {
		printf ("%s: expected 'one', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	if (NULL != elem->next) {
		printf ("%s: elem->next shoudl be NULL.\n", test_name);
		return 1;
	}
	if(list_p->count != 5) {
		printf ("%s: count should be 5.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_add_many)
{
	const int num_elements = 100000;
	struct llist *list_p;
	int i;

	list_p = create_llist();

	for (i = 0; i < num_elements; i++)
		prepend_element(list_p, &i);
	if (list_p->count != num_elements) {
		printf("count wrong: %d, expected %d.\n", list_p->count, num_elements);
		return 1;
	}
	printf("test_add_many ok.\n");
	return 0;
}
END_TEST	

START_TEST (test_add_struct)
{
	const int num_edges = 10;
	struct llist *list_p;
	int i;
	struct test_data *datap;
	struct list_elem *elem_p;

	list_p = create_llist();

	/* populate list */
	for (i = 0; i < num_edges; i++) {
		if (NULL == (datap = malloc(sizeof(struct test_data)))) {
			perror(NULL);
			return 1;
		}
		datap->height = (double) 2.5 * i;
		datap->length = (double) i;
		snprintf(datap->name, 10, "elem %d", i);
		prepend_element(list_p, (void *) datap);
	}
	
	/* check elements */
	for (elem_p=list_p->head, i=9; NULL != elem_p; elem_p = elem_p->next, i--) {
		datap = (struct test_data *) elem_p->data;
		if (datap->length != (double) i) {
			printf ("Expected edge length %.2f, got %.2f.",
					(double) i, datap->length);
			return 1;
		}
	}
	printf("test_add_struct ok.\n");
	return 0;
}
END_TEST

START_TEST (test_shallow_copy)
{
	char *test_name = "test_shallow_copy";
	struct llist *list_p, *list_copy_p;
	struct list_elem *elem;
	char label_two[] = {'t', 'w', 'o', '\0'};

	list_p = create_llist();
	append_element(list_p, "one");
	append_element(list_p, label_two); 	/* see below */
	append_element(list_p, "three");
	append_element(list_p, "four");
	append_element(list_p, "five");

	list_copy_p = shallow_copy(list_p);

	/* test list membership and count */
	elem = list_copy_p->head;
	if (strcmp(elem->data, "one") != 0) {
		printf ("%s: expected 'one', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "two") != 0) {
		printf ("%s: expected 'two', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "three") != 0) {
		printf ("%s: expected 'three', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "four") != 0) {
		printf ("%s: expected 'four', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = elem->next;
	if (strcmp(elem->data, "five") != 0) {
		printf ("%s: expected 'five', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	if(list_p->count != 5) {
		printf ("%s: count should be 5.\n", test_name);
		return 1;
	}

	/* Check that copy is shallow, i.e., original list members are the
	 * same a s copy members */
	/* String constants cannot be modified (SIGSEGV!), which is why we
	 * used an array for label #2 */

	elem = list_p->head->next; /* should work with any of them */
	label_two[0] = 'z';
	elem = list_copy_p->head->next;
	if (strcmp(elem->data, "zwo") != 0) {
		printf ("%s: expected element in copy to be 'zwo' (got '%s').\n", test_name, (char *) elem->data);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_shift)
{
	char *test_name = "test_shift";
	struct llist *list_p;
	struct list_elem *elem;

	list_p = create_llist();
	append_element(list_p, "one");
	append_element(list_p, "two");
	append_element(list_p, "three");
	append_element(list_p, "four");
	append_element(list_p, "five");

	elem = list_p->head;
	if (strcmp(elem->data, "one") != 0) {
		printf ("%s: expected 'one', got '%s'.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	if(list_p->count != 5) {
		printf ("%s: count should be 5.\n", test_name);
		return 1;
	}
	elem = shift(list_p);
	if (strcmp((char *) elem, "one") != 0) {
		printf ("%s: expected 'one', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	elem = list_p->head;
	if (strcmp(elem->data, "two") != 0) {
		printf ("%s: expected 'two', got %s.\n", test_name,
				(char *) elem->data);
		return 1;
	}
	if(list_p->count != 4) {
		printf ("%s: count should be 4.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

void * sum(void *a, void *b)
{
	int *r;

	if ((r = malloc(sizeof(int))) == NULL) {
		perror(NULL);
		exit(EXIT_FAILURE);
	}

	*r = *(int *)a + *(int *)b;
	return r;
}

START_TEST (test_reduce)
{
	char *test_name = "test_reduce";
	struct llist *list_p;
	int a = 1, b = 2, c = 3, d = 4, e = 5;
	void * result;

	list_p = create_llist();
	append_element(list_p, &a);
	append_element(list_p, &b);
	append_element(list_p, &c);
	append_element(list_p, &d);
	append_element(list_p, &e);

	result = reduce(list_p, sum);

	if (*((int *)result) != 15) {
		printf ("%s: expected 15, got %d.\n",
			test_name, *((int *)result));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_insert)
{
	const char *test_name = "test_insert";
	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "yksi");
	append_element(list1, "kaksi");
	append_element(list1, "kolme");
	append_element(list1, "yhdeksän");
	append_element(list1, "kymmenen");

	list2 = create_llist();
	append_element(list2, "neljä"); 
	append_element(list2, "viisi"); 
	append_element(list2, "kuusi"); 
	append_element(list2, "seitsemän"); 
	append_element(list2, "kahdeksan"); 

	insert_after(list1, 2, list2);

	struct list_elem *el;

	if (10 != list1->count) {
		printf ("%s: expected count of 10, got %d.\n", 
				test_name, list1->count);
		return 1;
	}
	el = list1->head;
	if (strcmp("yksi", (char *) el->data) != 0) {
		printf ("%s: expected 'yksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kaksi", (char *) el->data) != 0) {
		printf ("%s: expected 'kaksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kolme", (char *) el->data) != 0) {
		printf ("%s: expected 'kolme', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("neljä", (char *) el->data) != 0) {
		printf ("%s: expected 'neljä', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("viisi", (char *) el->data) != 0) {
		printf ("%s: expected 'viisi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kuusi", (char *) el->data) != 0) {
		printf ("%s: expected 'kuusi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("seitsemän", (char *) el->data) != 0) {
		printf ("%s: expected 'seitsemän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kahdeksan", (char *) el->data) != 0) {
		printf ("%s: expected 'kahdeksan', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yhdeksän", (char *) el->data) != 0) {
		printf ("%s: expected 'yhdeksän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kymmenen", (char *) el->data) != 0) {
		printf ("%s: expected 'kymmenen', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("kymmenen", (char *) list1->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'kymmenen', got '%s'.\n",
				test_name, (char *) list1->tail->data);
		return 1;
	}
	if (NULL != list1->tail->next) {
		printf ("%s: list1 is not terminated.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_insert_at_head)
{
	const char *test_name = "test_insert_at_head";

	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "yeoseot");
	append_element(list1, "ilgop");
	append_element(list1, "yeodeol");
	append_element(list1, "ahob");
	append_element(list1, "yeol");

	list2 = create_llist();
	append_element(list2, "hana"); 
	append_element(list2, "dul"); 
	append_element(list2, "set"); 
	append_element(list2, "net"); 
	append_element(list2, "daseot"); 

	insert_after(list1, -1, list2);

	struct list_elem *el;

	if (10 != list1->count) {
		printf ("%s: expected count of 10, got %d.\n", 
				test_name, list1->count);
		return 1;
	}
	el = list1->head;
	if (strcmp("hana", (char *) el->data) != 0) {
		printf ("%s: expected 'hana', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("dul", (char *) el->data) != 0) {
		printf ("%s: expected 'dul', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("set", (char *) el->data) != 0) {
		printf ("%s: expected 'set', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("net", (char *) el->data) != 0) {
		printf ("%s: expected 'net', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("daseot", (char *) el->data) != 0) {
		printf ("%s: expected 'daseot', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yeoseot", (char *) el->data) != 0) {
		printf ("%s: expected 'yeoseot', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("ilgop", (char *) el->data) != 0) {
		printf ("%s: expected 'ilgop', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yeodeol", (char *) el->data) != 0) {
		printf ("%s: expected 'yeodeol', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("ahob", (char *) el->data) != 0) {
		printf ("%s: expected 'ahob', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yeol", (char *) el->data) != 0) {
		printf ("%s: expected 'yeol', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("yeol", (char *) list1->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'yeol', got '%s'.\n",
				test_name, (char *) list1->tail->data);
		return 1;
	}
	if (NULL != list1->tail->next) {
		printf ("%s: list1 is not terminated.\n", test_name);
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_insert_at_tail)
{
	const char *test_name = "test_insert_at_tail";
	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "il");
	append_element(list1, "i");
	append_element(list1, "sam");
	append_element(list1, "sa");
	append_element(list1, "o");

	list2 = create_llist();
	append_element(list2, "yug"); 
	append_element(list2, "chil"); 
	append_element(list2, "pal"); 
	append_element(list2, "gu"); 
	append_element(list2, "shib"); 

	insert_after(list1, 4, list2);

	struct list_elem *el;

	if (10 != list1->count) {
		printf ("%s: expected count of 10, got %d.\n", 
				test_name, list1->count);
		return 1;
	}
	el = list1->head;
	if (strcmp("il", (char *) el->data) != 0) {
		printf ("%s: expected 'il', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("i", (char *) el->data) != 0) {
		printf ("%s: expected 'i', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("sam", (char *) el->data) != 0) {
		printf ("%s: expected 'sam', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("sa", (char *) el->data) != 0) {
		printf ("%s: expected 'sa', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("o", (char *) el->data) != 0) {
		printf ("%s: expected 'o', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yug", (char *) el->data) != 0) {
		printf ("%s: expected 'yug', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("chil", (char *) el->data) != 0) {
		printf ("%s: expected 'chil', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("pal", (char *) el->data) != 0) {
		printf ("%s: expected 'pal', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("gu", (char *) el->data) != 0) {
		printf ("%s: expected 'gu', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("shib", (char *) el->data) != 0) {
		printf ("%s: expected 'shib', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("shib", (char *) list1->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'shib', got '%s'.\n",
				test_name, (char *) list1->tail->data);
		return 1;
	}
	if (NULL != list1->tail->next) {
		printf ("%s: list1 is not terminated.\n", test_name);
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_delete)
{
	const char *test_name = "test_delete";
	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "yksi");
	append_element(list1, "kaksi");
	append_element(list1, "kolme");
	append_element(list1, "neljä"); 
	append_element(list1, "viisi"); 
	append_element(list1, "kuusi"); 
	append_element(list1, "seitsemän"); 
	append_element(list1, "kahdeksan"); 
	append_element(list1, "yhdeksän");
	append_element(list1, "kymmenen");

	list2 = delete_after(list1, 2, 3);

	struct list_elem *el;

	/* check list1 */
	if (7 != list1->count) {
		printf ("%s: expected count of 7, got %d.\n", 
				test_name, list1->count);
		return 1;
	}
	el = list1->head;
	if (strcmp("yksi", (char *) el->data) != 0) {
		printf ("%s: expected 'yksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kaksi", (char *) el->data) != 0) {
		printf ("%s: expected 'kaksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kolme", (char *) el->data) != 0) {
		printf ("%s: expected 'kolme', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("seitsemän", (char *) el->data) != 0) {
		printf ("%s: expected 'seitsemän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kahdeksan", (char *) el->data) != 0) {
		printf ("%s: expected 'kahdeksan', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yhdeksän", (char *) el->data) != 0) {
		printf ("%s: expected 'yhdeksän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kymmenen", (char *) el->data) != 0) {
		printf ("%s: expected 'kymmenen', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("kymmenen", (char *) list1->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'kymmenen', got '%s'.\n",
				test_name, (char *) list1->tail->data);
		return 1;
	}
	if (NULL != list1->tail->next) {
		printf ("%s: list1 is not terminated.\n", test_name);
		return 1;
	}

	/* check list2 */
	if (3 != list2->count) {
		printf ("%s: expected list count to be 3, got %d.\n",
				test_name, list2->count);
		return 1;
	}
	el = list2->head;
	if (strcmp("neljä", (char *) el->data) != 0) {
		printf ("%s: expected 'neljä', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("viisi", (char *) el->data) != 0) {
		printf ("%s: expected 'viisi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kuusi", (char *) el->data) != 0) {
		printf ("%s: expected 'kuusi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("kuusi", (char *) list2->tail->data) != 0) {
		printf ("%s: list tail to be 'kuusi', got '%s'.\n", test_name,
				(char *) list2->tail->data);
		return 1;
	}
	if (NULL != list2->tail->next) {
		printf ("%s: list2 is not terminated.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_delete_at_head)
{
	const char *test_name = "test_delete_at_head";
	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "yksi");
	append_element(list1, "kaksi");
	append_element(list1, "kolme");
	append_element(list1, "neljä"); 
	append_element(list1, "viisi"); 
	append_element(list1, "kuusi"); 
	append_element(list1, "seitsemän"); 
	append_element(list1, "kahdeksan"); 
	append_element(list1, "yhdeksän");
	append_element(list1, "kymmenen");

	list2 = delete_after(list1, -1, 3);

	struct list_elem *el;

	/* check list1 */
	if (7 != list1->count) {
		printf ("%s: expected count of 7, got %d.\n", 
				test_name, list1->count);
		return 1;
	}
	el = list1->head;
	if (strcmp("neljä", (char *) el->data) != 0) {
		printf ("%s: expected 'neljä', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("viisi", (char *) el->data) != 0) {
		printf ("%s: expected 'viisi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kuusi", (char *) el->data) != 0) {
		printf ("%s: expected 'kuusi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("seitsemän", (char *) el->data) != 0) {
		printf ("%s: expected 'seitsemän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kahdeksan", (char *) el->data) != 0) {
		printf ("%s: expected 'kahdeksan', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yhdeksän", (char *) el->data) != 0) {
		printf ("%s: expected 'yhdeksän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kymmenen", (char *) el->data) != 0) {
		printf ("%s: expected 'kymmenen', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("kymmenen", (char *) list1->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'kymmenen', got '%s'.\n",
				test_name, (char *) list1->tail->data);
		return 1;
	}
	if (NULL != list1->tail->next) {
		printf ("%s: list1 is not terminated.\n", test_name);
		return 1;
	}

	/* check list2 */
	if (3 != list2->count) {
		printf ("%s: expected list count to be 3, got %d.\n",
				test_name, list2->count);
		return 1;
	}
	el = list2->head;
	if (strcmp("yksi", (char *) el->data) != 0) {
		printf ("%s: expected 'yksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kaksi", (char *) el->data) != 0) {
		printf ("%s: expected 'kaksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kolme", (char *) el->data) != 0) {
		printf ("%s: expected 'kolme', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("kolme", (char *) list2->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'kolme', got '%s'.\n",
				test_name, (char *) list2->tail->data);
		return 1;
	}
	if (NULL != list2->tail->next) {
		printf ("%s: list2 is not terminated.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_index)
{
	const char *test_name = "test_index";
	struct llist *list1;

	list1 = create_llist();
	append_element(list1, "yksi");
	append_element(list1, "kaksi");
	append_element(list1, "kolme");
	append_element(list1, "neljä"); 
	append_element(list1, "viisi"); 
	append_element(list1, "kuusi"); 
	append_element(list1, "seitsemän"); 
	append_element(list1, "kahdeksan"); 
	append_element(list1, "yhdeksän");
	append_element(list1, "kymmenen");

	if (0 != llist_index_of(list1, "yksi")) {
		printf ("%s: expected index 0 for 'yksi', got %d.\n",
				test_name, llist_index_of(list1, "yksi"));
		return 1;
	}
	if (9 != llist_index_of(list1, "kymmenen")) {
		printf ("%s: expected index 9 for 'kymmenen', got %d.\n",
				test_name, llist_index_of(list1, "yksi"));
		return 1;
	}
	if (-1 != llist_index_of(list1, "roku")) {
		printf ("%s: expected index -1 (not found) for 'roku', got %d.\n",
				test_name, llist_index_of(list1, "roku"));
		return 1;
	}
	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_delete_at_tail)
{
	const char *test_name = "test_delete_at_tail";
	struct llist *list1;
	struct llist *list2;
	struct list_elem *el;

	list1 = create_llist();
	append_element(list1, "yksi");
	append_element(list1, "kaksi");
	append_element(list1, "kolme");
	append_element(list1, "neljä"); 
	append_element(list1, "viisi"); 
	append_element(list1, "kuusi"); 
	append_element(list1, "seitsemän"); 
	append_element(list1, "kahdeksan"); 
	append_element(list1, "yhdeksän");
	append_element(list1, "kymmenen");

	list2 = delete_after(list1, 6, 3);

	/* check list1 */
	if (7 != list1->count) {
		printf ("%s: expected count of 7, got %d.\n", 
				test_name, list1->count);
		return 1;
	}
	el = list1->head;
	if (strcmp("yksi", (char *) el->data) != 0) {
		printf ("%s: expected 'yksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kaksi", (char *) el->data) != 0) {
		printf ("%s: expected 'kaksi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kolme", (char *) el->data) != 0) {
		printf ("%s: expected 'kolme', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("neljä", (char *) el->data) != 0) {
		printf ("%s: expected 'neljä', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("viisi", (char *) el->data) != 0) {
		printf ("%s: expected 'viisi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kuusi", (char *) el->data) != 0) {
		printf ("%s: expected 'kuusi', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("seitsemän", (char *) el->data) != 0) {
		printf ("%s: expected 'seitsemän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("seitsemän", (char *) list1->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'seitsemän', got '%s'.\n",
				test_name, (char *) list1->tail->data);
		return 1;
	}
	if (NULL != list1->tail->next) {
		printf ("%s: list1 is not terminated.\n", test_name);
		return 1;
	}

	/* check list2 */
	if (3 != list2->count) {
		printf ("%s: expected count of 3, got %d.\n", 
				test_name, list2->count);
		return 1;
	}
	el = list2->head;
	if (strcmp("kahdeksan", (char *) el->data) != 0) {
		printf ("%s: expected 'kahdeksan', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("yhdeksän", (char *) el->data) != 0) {
		printf ("%s: expected 'yhdeksän', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	el = el->next;
	if (strcmp("kymmenen", (char *) el->data) != 0) {
		printf ("%s: expected 'kymmenen', got '%s'.\n", test_name,
				(char *) el->data);
		return 1;
	}
	if (strcmp("kymmenen", (char *) list2->tail->data) != 0) {
		printf ("%s: expexted list tail to be 'kymmenen', got '%s'.\n",
				test_name, (char *) list2->tail->data);
		return 1;
	}
	if (NULL != list2->tail->next) {
		printf ("%s: list2 is not terminated.\n", test_name);
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST

START_TEST (test_destroy)
{
	char *test_name = "test_destroy";
	struct llist *list_p;

	list_p = create_llist();
	prepend_element(list_p, "one");
	prepend_element(list_p, "two");
	prepend_element(list_p, "three");
	prepend_element(list_p, "four");
	prepend_element(list_p, "five");

	destroy_llist(list_p);

	printf("%s ok.\n", test_name);
	return 0;
}
END_TEST


