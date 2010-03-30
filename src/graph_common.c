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

double tick_interval(double x)
{
	int low_log10 = rint(log10(x));
	int high_log10 = low_log10 + 1;
	/* powers of ten below and above x, e.g. if x = 230, low_PoT = 100 and
	 * high_PoT = 1000. */
	double low_PoT = exp(low_log10 * log(10));
	double high_PoT = exp(high_log10 * log(10));
	printf ("lo PoT: %g, hi PoT: %g\n", low_PoT, high_PoT);

	/* We will divide the powers of ten by the following. This will yield
	 * "reasonable" potential intervals between tick bars with values like
	 * 10, 250, 500, etc. */
	int divisors[] = {2, 4, 5, 10};
	/* We prefer 4 or 5 divisions */
	int preferred_num_tick_intervals[] = {4,5};

	double penalty = INT_MAX;
	double best_tick_interval = -1;
	double worst_penalty = INT_MAX;
	int i = -1;
	for (i = 0; i < 4; i++) {
		double tick_interval = low_PoT / divisors[i];
		int num_tick_intervals = floor(x / tick_interval);
		double remainder = x - (num_tick_intervals * tick_interval);
		double relative_error = 100 * fabs(remainder / x);
		int j = -1;
		for (j = 0; j < 2; j++) {
			int tick_diff = abs(num_tick_intervals -
					preferred_num_tick_intervals[j]);
			penalty = 10 * tick_diff + relative_error;
			if (penalty < worst_penalty) {
				worst_penalty = penalty;
				best_tick_interval = tick_interval;
			}
			/*
			printf ("%g = %d * %g + %g (%d, %2.0f%% -> %g)\n",
					x, num_tick_intervals,
					tick_interval, remainder,
					tick_diff, relative_error, penalty);
			*/
		}
	}

	return best_tick_interval;
}
