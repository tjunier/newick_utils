#include <stdio.h>
#include <string.h>

#include "canvas.h"

int test_create()
{
	const char *test_name = "test_create";

	struct canvas *cp;
	const char *blank = "          ";	/* 10 ' ' */
	const int width = 10;
	const int height = 6;
	int i;

	cp = create_canvas(width, height);
	
	for (i = 0; i < height; i++)  {
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_getters()
{
	const char *test_name = __func__;

	struct canvas *cp;
	const char *blank = "          ";	/* 10 ' ' */
	const int width = 10;
	const int height = 6;
	int i;

	cp = create_canvas(width, height);

	if (width != get_canvas_width(cp)) {
		printf("%s: expected width of %d, got %d.\n",
				test_name, width, get_canvas_width(cp));
		return 1;
	}
	if (height != get_canvas_height(cp)) {
		printf("%s: expected height of %d, got %d.\n",
				test_name, height, get_canvas_height(cp));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_draw_h()
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
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}
	if (strcmp(_get_canvas_line(cp, 7), hline_10_16) != 0) {	
		printf ("%s: expected '%s', got '%s' (line %d)\n",
			test_name, hline_10_16, _get_canvas_line(cp, 7), 7);
		return 1;
	}
	for (i = 8; i < line_height; i++)  {
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_draw_v()
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
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}
	for (; i <= line_v_stop; i++)  {	/* <= intentional */ 
		if (strcmp(_get_canvas_line(cp, i), vline_14) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, vline_14, _get_canvas_line(cp, i), i);
			return 1;
		}
	}
	for (; i < height; i++)  {
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_write()
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
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}
	if (strcmp(_get_canvas_line(cp, 7), hline_10_16) != 0) {	
		printf ("%s: expected '%s', got '%s' (line %d)\n",
			test_name, hline_10_16, _get_canvas_line(cp, 7), 7);
		return 1;
	}
	for (i = 8; i < text_height; i++)  {
		if (strcmp(_get_canvas_line(cp, i), blank) != 0) {
			printf ("%s: expected '%s', got '%s' (line %d)\n",
				test_name, blank, _get_canvas_line(cp, i), i);
			return 1;
		}
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int test_cross()
{
	const char *test_name = "test_cross";

	struct canvas *cp;
	const char *l0 = "  +--";
	const char *l1 = "  |  ";
	const char *l2 = "--+- ";
	const char *l3 = "  |  ";

	cp = create_canvas(5, 4);
	canvas_draw_hline(cp, 0, 2, 4);
	canvas_draw_vline(cp, 2, 0, 3);
	canvas_draw_hline(cp, 2, 0, 3);
	
	if (strcmp(_get_canvas_line(cp, 0), l0) != 0) {
		printf ("%s: expected '%s', got '%s' (line 0)\n",
			test_name, l0, _get_canvas_line(cp, 0));
		return 1;
	}
	if (strcmp(_get_canvas_line(cp, 1), l1) != 0) {
		printf ("%s: expected '%s', got '%s' (line 1)\n",
			test_name, l1, _get_canvas_line(cp, 1));
		return 1;
	}
	if (strcmp(_get_canvas_line(cp, 2), l2) != 0) {
		printf ("%s: expected '%s', got '%s' (line 2)\n",
			test_name, l2, _get_canvas_line(cp, 2));
		return 1;
	}
	if (strcmp(_get_canvas_line(cp, 3), l3) != 0) {
		printf ("%s: expected '%s', got '%s' (line 3)\n",
			test_name, l3, _get_canvas_line(cp, 3));
		return 1;
	}

	printf("%s ok.\n", test_name);
	return 0;
}

int main()
{
	int failures = 0;
	printf("Starting canvas test...\n");
	failures += test_create();
	failures += test_getters();
	failures += test_draw_h();
	failures += test_draw_v();
	failures += test_write();
	failures += test_cross();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
