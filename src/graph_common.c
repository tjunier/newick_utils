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
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <limits.h>

#include "tree.h"
#include "list.h"
#include "rnode.h"

void underscores2spaces(char *s)
{
	char *p;
	for (p = s; '\0' != *p; p++)
		if ('_' == *p)
			*p = ' ';
}

void remove_quotes(char *s)
{
	char *tmp = strdup(s);
	char *c;
	int i = 0;

	for (c = tmp; '\0' != *c; c++)
		switch (*c) {
		// maybe a case for double quotes here later?
		case '\'':
			if ('\'' == *(c+1)) {
				s[i] = '\'';
				i++;
				c++;
			}
			break;
		default:
			s[i] = *c;
			i++;
		}
	s[i] = '\0';

	free(tmp);
}

void prettify_labels (struct rooted_tree *tree)
{
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		underscores2spaces(current->label);
		remove_quotes(current->label);
	}
}

/* This function takes a length and returns a smaller length that can be used
 * to build a scale bar ("tick interval"). Ideally, the tick interval should
 * meet the following criteria: i) the original length should be approximately
 * 4 or 5 intervals long; ii) the intervals should be a power of 10 divided by
 * 2, 4, 5, or 10 (i.e., only numbers like 10, 25, 500, 200 and their multiples
 * by a power of 10). */

double tick_interval(double x)
{
	int low_log10 = rint(log10(x));
	double low_PoT = exp(low_log10 * log(10));

	/* We will divide the powers of ten by the following. This will yield
	 * "reasonable" potential intervals between tick bars with values like
	 * 10, 250, 500, etc. */
	int divisors[] = {2, 4, 5, 10};
	/* We prefer 4 or 5 divisions */
	int preferred_num_tick_intervals[] = {4,5};

	int penalty = INT_MAX;
	double best_tick_interval = -1;
	int worst_penalty = INT_MAX;
	int i = -1;
	/* try each divisor in turn */
	for (i = 0; i < 4; i++) {
		/* a candidate interval */
		double tick_interval = low_PoT / divisors[i];
		/* we use floor() instead of rint() so that we never exceed x */
		int num_tick_intervals = floor(x / tick_interval);
		double remainder = x - (num_tick_intervals * tick_interval);
		/* error in % */
		int relative_error = rint(100 * fabs(remainder / x));
		int j = -1;
		for (j = 0; j < 2; j++) {
			/* difference between current preferred number of tick
			 * intevals and actual number of tick intervals */
			int tick_diff = abs(num_tick_intervals -
					preferred_num_tick_intervals[j]);
			penalty = 10 * tick_diff + relative_error;
			if (penalty < worst_penalty) {
				worst_penalty = penalty;
				best_tick_interval = tick_interval;
			}
		}
	}

	return best_tick_interval;
}
