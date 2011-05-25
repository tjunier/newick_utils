/* 

Copyright (c) 2010 Thomas Junier and Evgeny Zdobnov, University of Geneva
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

// TODO: See if this is also true with Lua.
/* NOTE: This program uses libguile. When checking for memory leaks with
 * Valgrind, I get error messages that seem to point to problems in libguile
 * rather than my own code. To see this, do e.g.:
 *
 * valgrind --leak-check=full \
 * ./src/nw_sched -n data/catarrhini '((= D 2) (s))' 2>&1 \
 * | grep -A 1 '^==[0-9]\+== [A-Z]' | grep -o '(in .*)' \
 * | tr -d '()' | awk '{print $2}' | sort -u
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "rnode.h"
#include "link.h"
#include "list.h"
#include "tree.h"
#include "parser.h"
#include "to_newick.h"
#include "tree_editor_rnode_data.h"
#include "common.h"
#include "masprintf.h"

struct rnode *current_node;

enum order { POST_ORDER, PRE_ORDER };

struct parameters {
	bool lua_on_CLI;	
	/* this can be either literal Lua code on the command line, or the
	 * name of a file that contains Lua code. This is governed by
	 * lua_on_CLI. */
	char *lua_test_list;	
	bool show_tree;
	int order;
	bool stop_clade_at_first_match;
	bool single;
};

void help(char *argv[])
{
	printf (
"Performs actions on nodes that match some condition, using an\n"
"embedded Lua interpreter. TODO: Update this help page!\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hnor] <newick trees filename|-> <Lua expression>\n"
"\n"
"NOTE: this program is still very experimental and will probably change!\n"
"\n"
"Input\n"
"-----\n"
"\n"
"First argument is the name of a file that contains Newick trees, or '-' (in\n"
"which case trees are read from standard input).\n"
"\n"
"Second argument is a Lua expression with two parts. The first part is\n"
"evaluated on each node in turn. If it evaluates to #t, then (and only then)\n"
"the second part is evaluated. The first part is typically used to select\n"
"nodes and is called the Selector, and the second part is used to perform\n"
"arbitrary actions and is called the Action. For example, the following\n"
"expression\n"
"\n"
"                                (#t (s))\n"
"\n"
"has '#t' as a selector and '(s)' as action. This selector is always true, so\n"
"the action (s) (print out the subtree rooted at the current node) is\n"
"performed for every node.\n"
"The program provides specalized Lua functions and variables for working\n"
"with nodes (see below).\n"
"\n"
"Output\n"
"------\n"
"\n"
"This program is analogous to pattern-oriented, stream processing UNIX\n"
"utilities like sed(1) and awk(1), but instead of working on lines (like\n"
"sed) or records (like awk), %s works on tree nodes.\n"
"By default, it prints the input tree, which may have been modified. However,\n"
"the action can cause arbitrary material to be printed out.\n"
"\n"
"Selector\n"
"---------\n"
"\n"
"The selector expression involves node properties such as depth, bootstrap\n"
"support, whether or not a node is a leaf, etc. These are represented by\n"
"top-level variables which are predefined for the current node. For example,\n"
"the predefined variable 'i' is true iff the current node is internal, while\n"
"variable 'b' is set to the current node's bootstrap support value\n"
"(assuming the the node has a label and that it is numeric).\n"
"For example,\n"
"\n"
"				 (and i (> b 75))\n"
"\n"
"will match internal nodes with a bootstrap support value greater than 75\n"
"(assuming that the support value is defined).\n"
"\n"
"The variables have short names to allow for compact expressions on the\n"
"command line.\n"
"\n"
"The predefined varables are:\n"
"\n"
"	Name	Type		Meaning\n"
"	---------------------------------------------------------\n"
"    	a	integer		number of ancestors\n"
"    	b	rational	support value\n"
"    	c	integer		number of children\n"
"    	d	rational	depth (distance to root)\n"
"    	D	integer		number of descendants\n"
"    	i	boolean    	true iff node is strictly internal\n"
"    	L	rational    	parent edge's length\n"
"    	l	boolean    	true iff node is a leaf\n"
"    	lbl	string    	label\n"
"    	r	boolean    	true iff node is the root\n"
"\n"
"Notes:\n"
"    	o Exactly one of i, l, and r is true for every node.\n"
"    	o The difference between b and lbl is that b interprets the label as\n"
"	  a number (if possible), while lbl returns the label as a string.\n"
"    	o Variables b, d, L and lbl may be undefined.\n"
"\n"
" The following Scheme forms and functions also have shorter names\n"
" predefined:\n"
"\n"
"    	and		&   	logical and\n"
"    	or		|   	logical or\n"
"	defined?	def?	#t iff argument is defined\n"
"\n"
"Actions\n"
"-------\n"
"\n"
"The action can be any valid Scheme expression. The program defines the\n"
"following functions that work with nodes. They all take the current node\n"
"as an implicit argument, and have undefined return type. Like variables,\n"
"they have short names to allow for concise expressions on the command line.\n"
"\n"
"	Name	Meaning\n"
"	---------------------------------------------------------\n"
"    	o 	'opens' a node - unlinks the node, and \n"
"			attaches children to the unliked node's parent\n"
"    	s 	prints the subtree rooted ad the current node\n"
"    	u 	unlinks the current node\n"
"\n"
"Function 'o' ('open') is useful for discarding poorly-supported nodes (see\n"
"Examples below).\n"
"\n"
"Setters\n"
".......\n"
"Label and length also have setter functions. They are named like the\n"
"corresponding variable, with a trailing '!' (to remind that they change\n"
"some data). They also have undefined return type.\n"
"\n"
"	Name	Arg type	Meaning\n"
"	---------------------------------------------------------\n"
"    	L!	rational	set the parent edge's length\n"
"    	lbl!	string		set the label\n"
"\n"
"In addition, the following general functions are defined\n"
"\n"
"	Name	Arg type	Meaning\n"
"	---------------------------------------------------------\n"
"    	p 	any		displays arg, then newline\n"
"				returns undefined.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -h: print this help text, and exit\n"
"    -n: do not print the (possibly modified) tree at the end of the run \n"
"        (modeled after sed -n)\n"
"    -r: visit tree in preorder (starting at root, and visiting a node\n"
"        before any of its descendants). Default is post-order (ends at root\n"
"        and visits a node after all its descendats).\n"
"    -o: stop processing a clade after the first match - that is, if a node\n"
"        matches, its descendants are not processed.\n"
"        Note: this option will automatically set -r, as it makes no\n"
"        sense in post-order.\n"
"\n"
"Bugs\n"
"----\n"
"\n"
"Although there are no known bugs in this program, it is to be considered\n"
"more experimental than the others.\n"
"\n"
"Examples\n"
"--------\n"
"\n"
"# \"open\" all nodes with bootstrap support <= 10 (assuming support is coded\n"
"# in internal node labels)\n"
"\n"
"$ %s data/HRV.bs.nw '((& i (<= b 10)) (o)) \n"
"\n"
"# \"open\" all nodes with bootstrap support < 750, then discard leaves that\n"
"# are directly attached to the ingroup's root. This effectively keeps only\n"
"# leaves that are part of well-supported clades.\n"
"\n"
"$ %s data/big.rn.nw '((& i (< b 750)) (o))' | %s - '((& l (= a 2)) (u))'\n"
"\n"
"# get all clades with at least one ancestor, 980 or better support. Do not\n"
"# print subtrees of matching clades, even if they match (option -o)\n"
"\n"
"$ %s data/big.rn.nw -n -o '((& (>= a 1) (>= b 980)) (s))'\n"
"\n"
"# format all defined lengths to 2 decimal places, instead of 6 in the\n"
"original:\n"
"\n"
"$ %s HRV.nw \"((def? 'L) (L! (format #f \\\"~,2f\\\" L)))\"\n"
"\n"
"# In the last example, I use def? to check that L (the length) is defined\n"
"# and when true, I use function L! to set the length to a new value, namely\n"
"# the result of the call to format.\n", 
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	);
}

static struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	params.lua_on_CLI = true;
	params.lua_test_list = NULL;
	params.show_tree = true;
	params.order = POST_ORDER;
	params.stop_clade_at_first_match = false;
	params.single = true;

	enum mult_values mult = MULT_UNSPECIFIED;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "f:hnm:or")) != -1) {
		switch (opt_char) {
		case 'f':
			params.lua_on_CLI = false;
			params.lua_test_list = optarg;
			params.single = false;
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'm':
			if (strcmp("1", optarg) == 0)
				mult = MULT_SINGLE;
			else
				mult = MULT_MULTIPLE;
			break;
		case 'n':
			params.show_tree = false;
			break;
		case 'o':
			params.stop_clade_at_first_match = true;
			params.order = PRE_ORDER;
			break;
		case 'r':
			params.order = PRE_ORDER;
			break;
		default:
			fprintf (stderr, "Unknown option '-%c'\n", opt_char);
			exit (EXIT_FAILURE);
		}
	}

	/* check arguments */
	if (3 >= (argc - optind) && argc > 1)	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		if (3 == (argc - optind)) {
			params.lua_condition = argv[optind+1];
			params.lua_action = argv[optind+2];
		}
	} else {
		fprintf(stderr, "Usage: %s [-hnro] <filename|-> "
				"<Scheme expression>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* A helper for parse_order_traversal() - gets the number of descendants of a
 * node. This function is NOT recursive, the node data of all descendants must
 * have been set already - which is why we're calling it from
 * parse_order_traversal().
 * */

static int get_nb_descendants(struct rnode *node)
{
	struct rnode *kid;
	struct rnode_data *rndata;
	int descendants = 0;

	for (kid = node->first_child; NULL != kid; kid = kid->next_sibling) {
		rndata = kid->data;
		descendants += rndata->nb_descendants;
		descendants += 1;	/* kid itself (no pun intended :-) ) */
	}

	return descendants;
}

/* This allocates the rnode_data structure for each node, and fills it with
 * "top-down" data,  i.e. data for which the parent's value needs to be known
 * (such as depth and number of ancestors). Some values do not depend on the
 * parent nor on the children, I also set them here (this is arbitrary, I
 * could set them in parse_order_traversal() below as well). */

static void reverse_parse_order_traversal(struct rooted_tree *tree)
{
	struct list_elem *el;
	struct llist *rev_nodes = llist_reverse(tree->nodes_in_order);
	if (NULL == rev_nodes) { perror(NULL), exit(EXIT_FAILURE); }
	struct rnode *node;
	struct rnode_data *rndata;

	el = rev_nodes->head;	/* root */
	node = (struct rnode *) el->data;
	rndata = malloc(sizeof(struct rnode_data));
	if (NULL == rndata) { perror(NULL); exit (EXIT_FAILURE); }
	rndata->nb_ancestors = 0;
	rndata->is_depth_defined = true;
	rndata->depth = 0;
	rndata->stop_mark = false;
	node->data = rndata;

	/* WARNING: don't forget to set values for the root's data, above. The
	 * following loop starts at the first non-root node! */

	for (el = rev_nodes->head->next; NULL != el; el = el -> next) {
		node = (struct rnode *) el->data;
		struct rnode_data *parent_data = node->parent->data;
		rndata = malloc(sizeof(struct rnode_data));
		if (NULL == rndata) { perror(NULL); exit (EXIT_FAILURE); }
		rndata->nb_ancestors = parent_data->nb_ancestors + 1;

		if (parent_data->is_depth_defined) {
			double edge_length;
			char *endptr;
			edge_length = strtod(node->edge_length_as_string,
					&endptr);
			if (endptr == node->edge_length_as_string) {
				/* no conversion: depth is undefined */
				rndata->is_depth_defined = false;
			} else {
				rndata->is_depth_defined = true; 
				rndata->depth = parent_data->depth
					+ edge_length;
			}
		} else {
			/* We don't even set the value: it must not be used. */
			rndata->is_depth_defined = false;
		}

		rndata->stop_mark = false;
		node->data = rndata;
	}

	destroy_llist(rev_nodes);
}

/* This fills bottom-up data. Note that it relies on rnode_data being already
 * allocated, which is done in reverse_parse_order_traversal(). Data that does
 * not depend on order is also filled in here. */

static void parse_order_traversal(struct rooted_tree *tree)
{
	struct list_elem *el;
	struct rnode *node;
	struct rnode_data *rndata;

	for (el = tree->nodes_in_order->head; NULL != el; el = el -> next) {
		node = (struct rnode *) el->data;
		rndata = (struct rnode_data *) node->data;
		rndata->support = atof(node->label);	
		rndata->nb_descendants = get_nb_descendants(node);
	}
}

/* Sets the value of the predefined variables (i, l, a, etc), according to the
 * node passed as argument. This is normally the current node, while visiting
 * the tree. Using variables rather than functions makes for shorter Scheme
 * expression can be shorter, because function calls can be replaced by
 * variables, e.g. (< 2 a) instead of (< 2 (a)) to check that the current node
 * has fewer than two ancestors. On the command line, this is handy. */

static void set_predefined_variables(struct rnode *node, lua_State *L)
{
	lua_pushnumber(L, 2.1);
	lua_setglobal(L, "n");

	/*
	SCM label = scm_from_locale_string(node->label);
	SCM edge_length_as_scm_string  = scm_from_locale_string(
			node->edge_length_as_string);
	scm_c_define("lbl", label);

		*/
	/* N: current node */
	//SCM node_smob = rnode_smob(node);
	//scm_c_define("N", node_smob);

	/* b: returns node label, as a bootstrap support value (or undefined if
	this can't be done) */
	//if (is_leaf(node))
		//scm_c_define("b", SCM_UNDEFINED);
	//else {
		//SCM support_value = scm_string_to_number(label, SCM_UNDEFINED);
		//if (SCM_BOOL_F == support_value)
			//scm_c_define("b", SCM_UNDEFINED);
		//else
			//scm_c_define("b", support_value);
	//}

	/* i: true IFF node is inner (not leaf, not root) */
	//if (is_inner_node(node))
		//scm_c_define("i", SCM_BOOL_T);
	//else
		//scm_c_define("i", SCM_BOOL_F);

	/* l: true IFF node is a leaf */
	//if (is_leaf(node))
		//scm_c_define("l", SCM_BOOL_T);
	//else
		//scm_c_define("l", SCM_BOOL_F);

	/* L: parent edge's length */
	//if (strcmp ("", node->edge_length_as_string) == 0) 
		//scm_c_define("L", SCM_UNDEFINED);
	//else	
		//scm_c_define("L", scm_string_to_number(
				//edge_length_as_scm_string, SCM_UNDEFINED));

	/* c: number of children */
	//scm_c_define("c", scm_from_int(current_node->child_count));
	
	//struct rnode_data *data = current_node->data;

	/* d: depth */
	//if (data->is_depth_defined)
		//scm_c_define("d", scm_from_double(data->depth));
	//else
		//scm_c_define("d", SCM_UNDEFINED);

	/* a: number of ancestors */
	//scm_c_define("a", scm_from_int(data->nb_ancestors));

	/* D: number of descendants */
	//scm_c_define("D", scm_from_int(data->nb_descendants));

	/* r: true iff node is root */
	/*
	if (is_root(node))
		scm_c_define("r", SCM_BOOL_T);
	else
		scm_c_define("r", SCM_BOOL_F);
		*/
}

static void process_tree(struct rooted_tree *tree, lua_State *L,
		struct parameters params)
{
	struct llist *nodes;
	struct list_elem *el;

	/* these two traversals fill the node data. */
	reverse_parse_order_traversal(tree);
	parse_order_traversal(tree);

	if (POST_ORDER == params.order)
		nodes = tree->nodes_in_order;
	else if (PRE_ORDER == params.order) {
		nodes = llist_reverse(tree->nodes_in_order);
		if (NULL == nodes) { perror(NULL); exit(EXIT_FAILURE); }
	}
	else 
		assert(0);	 /* programmer error... */


	/* Main loop: Iterate over all nodes */
	for (el = nodes->head; NULL != el; el = el -> next) {
		current_node = (struct rnode *) el->data;

		/* Check for stop mark in parent (see option -o) */
		if (! is_root(current_node)) { 	/* root has no parent... */
			if (((struct rnode_data *)
				current_node->parent->data)->stop_mark) {
				/* Stop-mark the current node and continue */ 
				((struct rnode_data *)
					current_node->data)->stop_mark = true;
				continue;
			}
		} 

		set_predefined_variables(current_node, L);
		const char *lua_code = "print (n)";
		const int code_len = strlen(lua_code);
		int error = luaL_loadbuffer(L, lua_code, code_len, "lua code")
			|| lua_pcall(L, 0, 0, 0);
		bool is_match = true; // TODO: call Lua condition, etc.

		if (is_match) {
			/* see -o switch */
			if (params.stop_clade_at_first_match)
				((struct rnode_data *)
				 current_node->data)->stop_mark = true;
		}

	} /* loop over all nodes */

	/* If order is PRE_ORDER, the list of nodes is an inverted copy, so we
	 * need to free it. */
	if (PRE_ORDER == params.order)
		destroy_llist(nodes);
}

int main(int argc, char* argv[])
{
	struct parameters params = get_params(argc, argv);
	struct rooted_tree *tree;

	/* Initializes Lua */
	lua_State *L = lua_open();   
	luaL_openlibs(L);

	const char *lua_code = "print (\"Hello, Newick!\")";
	const int code_len = strlen(lua_code);
	int error = luaL_loadbuffer(L, lua_code, code_len, "lua code") ||
		lua_pcall(L, 0, 0, 0);

	// run_phase_code(code_phase_alist, "start");
	while (NULL != (tree = parse_tree())) {
		//run_phase_code(code_phase_alist, "start-tree");
		process_tree(tree, L, params);
		if (params.show_tree) {
			dump_newick(tree->root); // TODO: faster f()?
		}
		destroy_tree(tree, NULL);
		//run_phase_code(code_phase_alist, "end-tree");
	}
	//run_phase_code(code_phase_alist, "end");

	exit(EXIT_SUCCESS);
}
