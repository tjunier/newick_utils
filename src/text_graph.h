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
/** \file
 * functions for displaying a text (ASCII) tree graph.
 */

#include <stdbool.h>

struct rooted_tree;
enum inner_lbl_pos;

/** Dumps a tree to stdout, as a text graph.
 * \arg \c tree the tree to dump
 * \arg \c width the maximum width (in columns, 80 is typical for a tty)
 * \arg \c align_leaves whether to align the leaves (cladograms)
 * \arg \c lbl_pos position of inner labels 
 * \arg \c with_scalebar whether or not to print a scale bar
 * \arg \c units scale bar units (only meaningful if \c with_scalebar is true)
 * \arg \c scale_zero_at_root zero is at the leaves (which should be aligned, useful e.g. for paleontological trees where 0 is present and time is in million years)
 * \return a display_status
 */

enum display_status display_tree(struct rooted_tree *tree,
		int width, bool align_leaves, enum inner_lbl_pos lbl_pos,
		bool with_scalebar, char *units, bool scale_zero_at_root);
