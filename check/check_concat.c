#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "../src/concat.h"

START_TEST (test_concat)
{
	char *test_name = "test_concat";
	char *s1 = "brobd";
	char *s2 = "urumq";

	fail_if (strcmp(concat(s1, s2), "brobdurumq") != 0, "concat failed");
}
END_TEST

Suite * concat_suite (void)
{
	Suite *s = suite_create ("concat");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test (tc_core, test_concat);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
	Suite *s = concat_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


