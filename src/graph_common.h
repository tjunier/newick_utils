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

/* These declarations are used by all modules involved in displaying trees. */

enum display_status { DISPLAY_OK, DISPLAY_UNKNOWN_STYLE, DISPLAY_MEM_ERROR };
enum inner_lbl_pos {INNER_LBL_LEAVES, INNER_LBL_MIDDLE, INNER_LBL_ROOT};

struct rooted_tree;

/* Replaces '_' by ' ' (changes argument!) */

void underscores2spaces(char *);

/* Removes (single) quotes (changes argument!) */

void remove_quotes(char *);

/* Prettify the labels by changing underscores to spaces and removing quotes.
 * Argument is changed. Do not call this before printing out Newick, or the
 * labels will be invalid. */

void prettify_labels(struct rooted_tree *);

/* Returns how many tick intervals can be fitted in a scale bar for a total
 * length of n */

double tick_interval(double x);
