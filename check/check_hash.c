#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "../src/hash.h"
#include "../src/list.h"

struct test_data {
	int value;
	char *name;
};

START_TEST(test_simple)
{
	char *test_value = "a string datum";
	struct hash *h;
	struct test_data td1 = {0, "myname"} ;

	h = create_hash(100);

	hash_set(h, "one", test_value);
	hash_set(h, "td1", &td1);

	char *str = (char *) hash_get(h, "one");
	fail_if (0 != strcmp(str, test_value), "got wrong value");
	struct test_data *tdr = (struct test_data *) hash_get(h, "td1");
	fail_if (0 != tdr->value, "got wrong value");
	fail_if (0 != strcmp("myname", tdr->name), "got wrong value");
}
END_TEST

START_TEST(test_clash)
{
	struct hash *h = create_hash(4);

	hash_set(h, "one", "uno");
	hash_set(h, "two", "dos");
	hash_set(h, "three", "tres");
	hash_set(h, "four", "cuatro");
	/* one more elem than hash size - forces clash */
	hash_set(h, "five", "cinco"); 	

	struct llist *keys = hash_keys(h);

	fail_if (5 != keys->count, "expected 5 keys");
	fail_if (-1 == llist_index_of(keys, "one"), "'one' should be among the keys");
	fail_if (-1 == llist_index_of(keys, "two"), "'two' should be among the keys");
	fail_if (-1 == llist_index_of(keys, "three"), "'three' should be among the keys");
	fail_if (-1 == llist_index_of(keys, "four"), "'four' should be among the keys");
	fail_if (-1 == llist_index_of(keys, "five"), "'five' should be among the keys");
}
END_TEST

Suite * hash_suite (void)
{
	Suite *s = suite_create ("hash");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test(tc_core, test_simple);
	tcase_add_test(tc_core, test_clash);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
	Suite *s = hash_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


