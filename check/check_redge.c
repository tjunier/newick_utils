#include <stdlib.h>
#include <check.h>

#include "../src/redge.h"

START_TEST (test_create)
{
	struct redge *edgep;
	char *length = "12.345";
	edgep = create_redge(length);
	fail_if (NULL == edgep, "edge created NULL");
	fail_if (strcmp(edgep->length_as_string, length) != 0,
			"wrong length");
}
END_TEST

START_TEST (test_create_null_length)
{
	struct redge *edgep;
	edgep = create_redge(NULL);
	fail_if (0 != strcmp("", edgep->length_as_string),
			"length not empty");
}
END_TEST

START_TEST (test_create_empty_length)
{
	struct redge *edgep;
	edgep = create_redge("");
	fail_if (0 != strcmp("", edgep->length_as_string),
			"length not empty");
}
END_TEST

START_TEST (test_create_many)
{
	int i;
	int num = 100000;
	for (i = 0; i < num; i++) {
		struct redge *edgep;
		edgep = create_redge("test");
	}
}
END_TEST

Suite * redge_suite (void)
{
	Suite *s = suite_create ("redge");

	TCase *tc_core = tcase_create ("creation");
	tcase_add_test (tc_core, test_create);
	tcase_add_test (tc_core, test_create_null_length);
	tcase_add_test (tc_core, test_create_empty_length);
	tcase_add_test (tc_core, test_create_many);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
       Suite *s = redge_suite ();
       SRunner *sr = srunner_create (s);
       srunner_run_all (sr, CK_VERBOSE);
       number_failed = srunner_ntests_failed (sr);
       srunner_free (sr);
       return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


