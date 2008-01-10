#include <stdlib.h>
#include <string.h>
#include <check.h>

#include "../src/canvas.h"

START_TEST(test_create)
{
	const char *test_name = "test_create";

	struct canvas *cp;
	const char *blank = "          ";	/* 10 ' ' */
	const int width = 10;
	const int height = 6;
	int i;

	cp = create_canvas(width, height);
	
	for (i = 0; i < height; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "expected empty line");
	}
}
END_TEST

START_TEST(test_draw_h)
{
	const char *test_name = "test_draw_h";

	struct canvas *cp;
	const char *blank = "                    ";	/* 20 ' ' */
	const char *hline_10_16 = "          -------   ";
	const int width = 20;
	const int height = 12;
	const int line_height = 7;
	const int line_col_start = 10;
	const int line_col_stop = 16;
	int i;

	cp = create_canvas(width, height);
	canvas_draw_hline(cp, line_height, line_col_start, line_col_stop);
	
	for (i = 0; i < line_height; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "wrong line");
	}
	fail_if (strcmp(cp->lines[7], hline_10_16) != 0, "wrong line");
	for (i = 8; i < line_height; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "wrong line");
	}
}
END_TEST

START_TEST(test_draw_v)
{
	const char *test_name = "test_draw_v";

	struct canvas *cp;
	const int width = 20;
	const int height = 12;
	const int col_hpos = 14;
	const int line_v_start = 4;
	const int line_v_stop = 9;
	const char *blank = "                    ";	/* 20 ' ' */
	const char *vline_14 = "              |     ";
	int i;

	cp = create_canvas(width, height);
	canvas_draw_vline(cp, col_hpos, line_v_start, line_v_stop);
	
	for (i = 0; i < line_v_start; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "wrong line");
	}
	for (; i <= line_v_stop; i++)  {	/* <= intentional */ 
		fail_if (strcmp(cp->lines[i], vline_14) != 0, "wrong line");
	}
	for (; i < height; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "wrong line");
	}
}
END_TEST

START_TEST(test_write)
{
	const char *test_name = "test_write";

	struct canvas *cp;
	const char *blank = "                    ";	/* 20 ' ' */
	const char *hline_10_16 = "          bulgone   ";
	const int width = 20;
	const int height = 12;
	const int text_height = 7;
	const int text_col_start = 10;
	int i;

	cp = create_canvas(width, height);
	canvas_write(cp, text_col_start, text_height, "bulgone");
	
	for (i = 0; i < text_height; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "wrong line");
	}
	fail_if (strcmp(cp->lines[7], hline_10_16) != 0, "wrong line");
	for (i = 8; i < height; i++)  {
		fail_if (strcmp(cp->lines[i], blank) != 0, "wrong line");
	}
}
END_TEST

Suite * canvas_suite (void)
{
	Suite *s = suite_create ("canvas");

	TCase *tc_core = tcase_create ("main");
	tcase_add_test(tc_core, test_create);
	tcase_add_test(tc_core, test_draw_h);
	tcase_add_test(tc_core, test_draw_v);
	tcase_add_test(tc_core, test_write);
	suite_add_tcase (s, tc_core);

	return s;
}

int main (void)
{
   	int number_failed;
	Suite *s = canvas_suite ();
	SRunner *sr = srunner_create (s);
	srunner_run_all (sr, CK_ENV);
	number_failed = srunner_ntests_failed (sr);
	srunner_free (sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


