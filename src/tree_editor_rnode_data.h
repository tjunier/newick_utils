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
 * Data structure for \c nw_ed.
 * \deprecated - see \c nw_sched */

#include <stdbool.h>

/** \cond -- this is obsolete, so don't show. */

/* This is a structure that can hold many node properties. It is used e.g. in
 * the tree editor, when the properties that will actually be used are not known
 * in advance: the nodes are first traversed to fill this structure.  Contrast
 * this with e.g. displaying: the node properties that we need are their
 * horizontal and vertical positions, and only those. The 'is_depth_defined' 
 * field can be used to mark undefined depths. This is because the depth is a
 * double, and there is no double value that can signal undefinedness -- zero is
 * a valid defined depth, and as mentioned elsewhere, we can't use a negative
 * number to signal undefined depths, because some tree-building algorithms
 * (like NJ) may produce negative lengths. */

struct rnode_data {
	float support;		/**< normally derived from label */
	float depth;		/**< from root; normally root's depth == 0 */
	bool is_depth_defined;	/**< false if depth is undefined */
	int nb_ancestors;	/**< root has 0 */
	int nb_descendants;	/**< direct (children) and indirect */
	bool stop_mark;		/**< to stop processing of a clade, see option -o */
};

/** \endcond */
