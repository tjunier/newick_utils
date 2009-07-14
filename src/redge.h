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

struct rnode;

struct redge {
	struct rnode * parent_node;
	struct rnode * child_node;
	char *length_as_string;	/* Most of the time this is enough, but... */
	double length; 		/* ...use this when numerical value is needed */
};

/* Creates a redge and returns a pointer to it (checks malloc()) or dies.
 * Argument is the edge's length, which is stored as a string but should be
 * numeric or empty (NULL will be replaced by ""). This is because we need a
 * way to store unspecified length, and cannot use numbers for this (zero is a
 * possible numeric length, and even negative numbers are sometimes used (e.g.
 * by Neighbor-Joining))
 * The argument string is copied to a local, dynamically allocated version.
 * There is no destructor function (yet?) so the copy must be free()d (and the
 * original too IFF it was dynamically allocated, of course) */

struct redge *create_redge(char *length);

/* Frees the memory used by an edge */

void destroy_redge(struct redge *);

/* A dump function, for use with dump_llist() */

void dump_redge(void *);
