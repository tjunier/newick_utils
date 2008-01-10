#include <stdlib.h>
#include <stdio.h>
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
	fail_if (strcmp((char *) list_p->tail->data, "one"), "wrong data");
	fail_if (list_p->tail->next, "list is not terminated");
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
	fail_if (strcmp((char *) elem->data, "one"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "two"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "three"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "four"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "five"), "wrong data");
	fail_if (strcmp((char *) list_p->tail->data, "five"), "wrong data");
	fail_if (list_p->tail->next, "list is not terminated");
	fail_if (5 != list_p->count, "wrong count");
}
END_TEST

START_TEST (test_reverse)
{
	struct llist *list_p, *revlist_p;
	struct list_elem *elem;

	list_p = create_llist();
	append_element(list_p, "one");
	append_element(list_p, "two");
	append_element(list_p, "three");
	append_element(list_p, "four");
	append_element(list_p, "five");

	revlist_p = llist_reverse(list_p);

	fail_if (5 != revlist_p->count, "wrong count");
	elem = revlist_p->head;
	fail_if (strcmp((char *) elem->data, "five"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "four"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "three"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "two"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "one"), "wrong data");
	fail_if (strcmp((char *) revlist_p->tail->data, "one"), "wrong data");
	fail_if (revlist_p->tail->next, "list is not terminated");
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
	fail_if (list_p->count != num_elements, "wrong number of elements");
}
END_TEST	

START_TEST (test_add_struct)
{
	const int num_data = 10;
	struct llist *list_p;
	int i;
	struct test_data *datap;
	struct list_elem *elem_p;

	list_p = create_llist();

	/* populate list */
	for (i = 0; i < num_data; i++) {
		if (NULL == (datap = malloc(sizeof(struct test_data)))) {
			perror(NULL);
		}
		datap->height = (double) 2.5 * i;
		datap->length = (double) i;
		snprintf(datap->name, 10, "elem %d", i);
		prepend_element(list_p, (void *) datap);
	}
	
	/* check elements */
	for (elem_p=list_p->head, i=9; NULL != elem_p; elem_p = elem_p->next, i--) {
		datap = (struct test_data *) elem_p->data;
		fail_if (datap->length != (double) i, "wrong edge length");
	}
}
END_TEST

START_TEST (test_shallow_copy)
{
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
	fail_if (5 != list_p->count, "wrong count");
	fail_if (5 != list_copy_p->count, "wrong count");
	elem = list_copy_p->head;
	fail_if (strcmp((char *) elem->data, "one"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "two"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "three"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "four"), "wrong data");
	elem = elem->next;
	fail_if (strcmp((char *) elem->data, "five"), "wrong data");
	fail_if (strcmp((char *) list_copy_p->tail->data, "five"), "wrong data");
	fail_if (list_copy_p->tail->next, "list is not terminated");

	/* Check that copy is shallow, i.e., original list members are the
	 * same a s copy members */
	/* String constants cannot be modified (SIGSEGV!), which is why we
	 * used an array for label #2 */

	elem = list_p->head->next; /* should work with any of them */
	label_two[0] = 'z';
	elem = list_copy_p->head->next;
	fail_if (strcmp(elem->data, "zwo") != 0, "shallow copy failed");
}
END_TEST

START_TEST (test_shift)
{
	struct llist *list_p;
	struct list_elem *elem;

	list_p = create_llist();
	append_element(list_p, "one");
	append_element(list_p, "two");
	append_element(list_p, "three");
	append_element(list_p, "four");
	append_element(list_p, "five");

	elem = list_p->head;
	fail_if(strcmp(elem->data, "one") != 0, "head is not 'one'");
	fail_if(list_p->count != 5, "count is not 5");
	elem = shift(list_p);
	fail_if (strcmp((char *) elem, "one") != 0, "shifted elem is not 'one'");
	elem = list_p->head;
	fail_if (strcmp(elem->data, "two") != 0, "head is not 'two'");
	fail_if(list_p->count != 4, "count should be 4.");
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

	fail_if (*((int *)result) != 15, "expected 15");
}
END_TEST

START_TEST (test_insert)
{
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

	fail_if (10 != list1->count, "expected count of 10");
	el = list1->head;
	fail_if (strcmp("yksi", (char *) el->data) != 0, "expected 'yksi'");
	el = el->next;
	fail_if (strcmp("kaksi", (char *) el->data) != 0, "expected 'kaksi'");
	el = el->next;
	fail_if (strcmp("kolme", (char *) el->data) != 0, "expected 'kolme'");
	el = el->next;
	fail_if (strcmp("neljä", (char *) el->data) != 0, "expected 'neljä'");
	el = el->next;
	fail_if (strcmp("viisi", (char *) el->data) != 0, "expected 'viisi'");
	el = el->next;
	fail_if (strcmp("kuusi", (char *) el->data) != 0, "expected 'kuusi'");
	el = el->next;
	fail_if (strcmp("seitsemän", (char *) el->data) != 0, "expected 'seitsemän'");
	el = el->next;
	fail_if (strcmp("kahdeksan", (char *) el->data) != 0, "expected 'kahdeksan'");
	el = el->next;
	fail_if (strcmp("yhdeksän", (char *) el->data) != 0, "expected 'yhdeksän'");
	el = el->next;
	fail_if (strcmp("kymmenen", (char *) el->data) != 0, "expected 'kymmenen'");
	fail_if (strcmp((char *) list1->tail->data, "kymmenen"), "wrong data");
	fail_if (NULL != list1->tail->next, "list is not terminated");
}
END_TEST

START_TEST (test_insert_at_head)
{
	const char *test_name = "test_insert_at_head";

	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "kuusi");
	append_element(list1, "seitsemän");
	append_element(list1, "kahdeksan");
	append_element(list1, "yhdeksän");
	append_element(list1, "kymmenen");

	list2 = create_llist();
	append_element(list2, "yksi"); 
	append_element(list2, "kaksi"); 
	append_element(list2, "kolme"); 
	append_element(list2, "neljä"); 
	append_element(list2, "viisi"); 

	insert_after(list1, -1, list2);

	struct list_elem *el;

	fail_if (10 != list1->count, "expected count of 10");
	el = list1->head;
	fail_if (strcmp("yksi", (char *) el->data) != 0, "expected 'yksi'");
	el = el->next;
	fail_if (strcmp("kaksi", (char *) el->data) != 0, "expected 'kaksi'");
	el = el->next;
	fail_if (strcmp("kolme", (char *) el->data) != 0, "expected 'kolme'");
	el = el->next;
	fail_if (strcmp("neljä", (char *) el->data) != 0, "expected 'neljä'");
	el = el->next;
	fail_if (strcmp("viisi", (char *) el->data) != 0, "expected 'viisi'");
	el = el->next;
	fail_if (strcmp("kuusi", (char *) el->data) != 0, "expected 'kuusi'");
	el = el->next;
	fail_if (strcmp("seitsemän", (char *) el->data) != 0, "expected 'seitsemän'");
	el = el->next;
	fail_if (strcmp("kahdeksan", (char *) el->data) != 0, "expected 'kahdeksan'");
	el = el->next;
	fail_if (strcmp("yhdeksän", (char *) el->data) != 0, "expected 'yhdeksän'");
	el = el->next;
	fail_if (strcmp("kymmenen", (char *) el->data) != 0, "expected 'kymmenen'");
	fail_if (strcmp((char *) list1->tail->data, "kymmenen"), "wrong data");
	fail_if (NULL != list1->tail->next, "list is not terminated");
}
END_TEST

START_TEST (test_insert_at_tail)
{
	const char *test_name = "test_insert_at_tail";
	struct llist *list1;
	struct llist *list2;

	list1 = create_llist();
	append_element(list1, "yksi");
	append_element(list1, "kaksi");
	append_element(list1, "kolme");
	append_element(list1, "neljä");
	append_element(list1, "viisi");

	list2 = create_llist();
	append_element(list2, "kuusi"); 
	append_element(list2, "seitsemän"); 
	append_element(list2, "kahdeksan"); 
	append_element(list2, "yhdeksän"); 
	append_element(list2, "kymmenen"); 

	insert_after(list1, 4, list2);

	struct list_elem *el;

	fail_if (10 != list1->count, "expected count of 10");
	el = list1->head;
	fail_if (strcmp("yksi", (char *) el->data) != 0, "expected 'yksi'");
	el = el->next;
	fail_if (strcmp("kaksi", (char *) el->data) != 0, "expected 'kaksi'");
	el = el->next;
	fail_if (strcmp("kolme", (char *) el->data) != 0, "expected 'kolme'");
	el = el->next;
	fail_if (strcmp("neljä", (char *) el->data) != 0, "expected 'neljä'");
	el = el->next;
	fail_if (strcmp("viisi", (char *) el->data) != 0, "expected 'viisi'");
	el = el->next;
	fail_if (strcmp("kuusi", (char *) el->data) != 0, "expected 'kuusi'");
	el = el->next;
	fail_if (strcmp("seitsemän", (char *) el->data) != 0, "expected 'seitsemän'");
	el = el->next;
	fail_if (strcmp("kahdeksan", (char *) el->data) != 0, "expected 'kahdeksan'");
	el = el->next;
	fail_if (strcmp("yhdeksän", (char *) el->data) != 0, "expected 'yhdeksän'");
	el = el->next;
	fail_if (strcmp("kymmenen", (char *) el->data) != 0, "expected 'kymmenen'");
	fail_if (strcmp((char *) list1->tail->data, "kymmenen"), "wrong data");
	fail_if (NULL != list1->tail->next, "list is not terminated");

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
	fail_if (7 != list1->count, "expected count of 7");
	el = list1->head;
	fail_if (strcmp("yksi", (char *) el->data) != 0, "expected 'yksi'");
	el = el->next;
	fail_if (strcmp("kaksi", (char *) el->data) != 0, "expected 'kaksi'");
	el = el->next;
	fail_if (strcmp("kolme", (char *) el->data) != 0, "expected 'kolme'");
	el = el->next;
	fail_if (strcmp("seitsemän", (char *) el->data) != 0, "expected 'seitsemän'");
	el = el->next;
	fail_if (strcmp("kahdeksan", (char *) el->data) != 0, "expected 'kahdeksan'");
	el = el->next;
	fail_if (strcmp("yhdeksän", (char *) el->data) != 0, "expected 'yhdeksän'");
	el = el->next;
	fail_if (strcmp("kymmenen", (char *) el->data) != 0, "expected 'kymmenen'");
	fail_if (strcmp((char *) list1->tail->data, "kymmenen"), "wrong data");
	fail_if (NULL != list1->tail->next, "list is not terminated");

	/* check list2 */
	fail_if (3 != list2->count, "expected count of 3");
	el = list2->head;
	fail_if (strcmp("neljä", (char *) el->data) != 0, "expected 'neljä'");
	el = el->next;
	fail_if (strcmp("viisi", (char *) el->data) != 0, "expected 'viisi'");
	el = el->next;
	fail_if (strcmp("kuusi", (char *) el->data) != 0, "expected 'kuusi'");
	fail_if (strcmp((char *) list2->tail->data, "kuusi"), "wrong data");
	fail_if (NULL != list2->tail->next, "list is not terminated");
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
	fail_if (7 != list1->count, "expected count of 7");
	el = list1->head;
	fail_if (strcmp("neljä", (char *) el->data) != 0, "expected 'neljä'");
	el = el->next;
	fail_if (strcmp("viisi", (char *) el->data) != 0, "expected 'viisi'");
	el = el->next;
	fail_if (strcmp("kuusi", (char *) el->data) != 0, "expected 'kuusi'");
	el = el->next;
	fail_if (strcmp("seitsemän", (char *) el->data) != 0, "expected 'seitsemän'");
	el = el->next;
	fail_if (strcmp("kahdeksan", (char *) el->data) != 0, "expected 'kahdeksan'");
	el = el->next;
	fail_if (strcmp("yhdeksän", (char *) el->data) != 0, "expected 'yhdeksän'");
	el = el->next;
	fail_if (strcmp("kymmenen", (char *) el->data) != 0, "expected 'kymmenen'");
	fail_if (strcmp((char *) list1->tail->data, "kymmenen"), "wrong data");
	fail_if (NULL != list1->tail->next, "list is not terminated");

	/* check list2 */
	fail_if (3 != list2->count, "expected count of 3");
	el = list2->head;
	fail_if (strcmp("yksi", (char *) el->data) != 0, "expected 'yksi'");
	el = el->next;
	fail_if (strcmp("kaksi", (char *) el->data) != 0, "expected 'kaksi'");
	el = el->next;
	fail_if (strcmp("kolme", (char *) el->data) != 0, "expected 'kolme'");
	fail_if (strcmp((char *) list2->tail->data, "kolme"), "wrong data");
	fail_if (NULL != list2->tail->next, "list is not terminated");
}
END_TEST

START_TEST (test_index)
{
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

	fail_if (0 != llist_index_of(list1, "yksi"), "expected index 0 for 'yksi'");
	fail_if (9 != llist_index_of(list1, "kymmenen"), "expected index 9 for 'kymmenen'");
	fail_if (-1 != llist_index_of(list1, "roku"), "expected index -1 for 'roku'");
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
	fail_if (7 != list1->count, "expected count of 7");
	el = list1->head;
	fail_if (strcmp("yksi", (char *) el->data) != 0, "expected 'yksi'");
	el = el->next;
	fail_if (strcmp("kaksi", (char *) el->data) != 0, "expected 'kaksi'");
	el = el->next;
	fail_if (strcmp("kolme", (char *) el->data) != 0, "expected 'kolme'");
	el = el->next;
	fail_if (strcmp("neljä", (char *) el->data) != 0, "expected 'neljä'");
	el = el->next;
	fail_if (strcmp("viisi", (char *) el->data) != 0, "expected 'viisi'");
	el = el->next;
	fail_if (strcmp("kuusi", (char *) el->data) != 0, "expected 'kuusi'");
	el = el->next;
	fail_if (strcmp("seitsemän", (char *) el->data) != 0, "expected 'seitsemän'");
	fail_if (strcmp((char *) list1->tail->data, "seitsemän"), "wrong data");
	fail_if (NULL != list1->tail->next, "list is not terminated");

	/* check list2 */
	fail_if (3 != list2->count, "expected count of 3");
	el = list2->head;
	fail_if (strcmp("kahdeksan", (char *) el->data) != 0, "expected 'kahdeksan'");
	el = el->next;
	fail_if (strcmp("yhdeksän", (char *) el->data) != 0, "expected 'yhdeksän'");
	el = el->next;
	fail_if (strcmp("kymmenen", (char *) el->data) != 0, "expected 'kymmenen'");
	fail_if (strcmp((char *) list2->tail->data, "kymmenen"), "wrong data");
	fail_if (NULL != list2->tail->next, "list is not terminated");
}
END_TEST

START_TEST (test_destroy)
{
	struct llist *list_p;

	list_p = create_llist();
	prepend_element(list_p, "one");
	prepend_element(list_p, "two");
	prepend_element(list_p, "three");
	prepend_element(list_p, "four");
	prepend_element(list_p, "five");

	destroy_llist(list_p);
}
END_TEST

Suite * llist_suite (void)
{
	Suite *s = suite_create ("llist");

	TCase *tc_creat_destr = tcase_create ("creation-destruction");
	tcase_add_test (tc_creat_destr, test_create);
	tcase_add_test (tc_creat_destr, test_destroy);
	suite_add_tcase (s, tc_creat_destr);

	TCase *tc_insert = tcase_create ("element insertion");
	tcase_add_test (tc_insert, test_prepend_1_element);
	tcase_add_test (tc_insert, test_prepend_five);
	tcase_add_test (tc_insert, test_append_1_element);
	tcase_add_test (tc_insert, test_append_five);
	tcase_add_test (tc_insert, test_add_struct);
	tcase_add_test (tc_insert, test_add_many);
	suite_add_tcase (s, tc_insert);

	TCase *tc_del = tcase_create ("element deletion");
	tcase_add_test (tc_del, test_shift);
	suite_add_tcase (s, tc_del);

	TCase *tc_linsert = tcase_create ("list insertion");
	tcase_add_test (tc_linsert, test_insert);
	tcase_add_test (tc_linsert, test_insert_at_head);
	tcase_add_test (tc_linsert, test_insert_at_tail);
	suite_add_tcase (s, tc_linsert);

	TCase *tc_ldel = tcase_create ("list deletion");
	tcase_add_test (tc_ldel, test_delete);
	tcase_add_test (tc_ldel, test_delete_at_head);
	tcase_add_test (tc_ldel, test_delete_at_tail);
	suite_add_tcase (s, tc_ldel);

	TCase *tc_ops = tcase_create ("list operations");
	tcase_add_test (tc_ops, test_reduce);
	tcase_add_test (tc_ops, test_reverse);
	tcase_add_test (tc_ops, test_shallow_copy);
	tcase_add_test (tc_ops, test_index);
	suite_add_tcase (s, tc_ops);

	return s;
}

int main (void)
{
   	int number_failed;
       Suite *s = llist_suite ();
       SRunner *sr = srunner_create (s);
       srunner_run_all (sr, CK_ENV);
       number_failed = srunner_ntests_failed (sr);
       srunner_free (sr);
       return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


