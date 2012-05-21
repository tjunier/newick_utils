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

enum parser_status_type {
	PARSER_STATUS_OK,
	PARSER_STATUS_EMPTY,
	PARSER_STATUS_PARSE_ERROR,
	PARSER_STATUS_MALLOC_ERROR
};
struct rooted_tree;

extern enum parser_status_type newick_parser_status;

/* Sets the parser's input to the file whose name is passed as argument.
 * Returns FAILURE iff there was a problem (file not found, or error, etc).
 * */

int set_parser_input_filename (char *filename);

/* Parses a tree from nwsin, returns a pointer to a tree structure, or NULL if
 * there is no input. It is the caller's responsibility to set nwsin (which by
 * default is stdin). Use one of the set_parser_input_*() functions.. */

struct rooted_tree *parse_tree();
