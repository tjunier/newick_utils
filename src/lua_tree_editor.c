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

const char *CONDITION = "condition";
const char *ACTION = "action";
const char *STOP_AT_1ST_MATCH = "stop";
const char *START = "start_run";
const char *START_TREE = "start_tree";
const char *NODE = "node";
const char *STOP_TREE = "stop_tree";
const char *STOP = "stop_run";

enum order { POST_ORDER, PRE_ORDER };

enum node_field { UNKNOWN_FIELD, NODE_LABEL, NODE_SUPPORT, NODE_LENGTH,
	NODE_IS_LEAF, NODE_IS_INNER, NOIDE_IS_ROOT, NODE_PARENT,
	NODE_FIRST_CHILD, NODE_CHILDREN, NODE_LAST_CHILD, NODE_CHILD_COUNT};

struct parameters {
	char *lua_action;	
	char *lua_condition;	
	char *user_hooks_filename;
	bool show_tree;
	int order;
	bool stop_clade_at_first_match;
	bool single;
};

struct lua_rnode {
	struct rnode *orig;	/* Just keep a ptr to the original rnode */
};

static void help(char *argv[])
{
	printf (
"Performs actions on nodes that match some condition, using an\n"
"embedded Lua interpreter.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [-hnor] <newick trees filename|-> <selector> <action>\n"
"\n"
"or\n"
"\n"
"%s [-hnor] -f <filename> <newick trees filename|->\n"
"\n"
"This program is analogous to pattern-oriented, stream processing UNIX\n"
"utilities like sed(1) and awk(1), but instead of working on lines (like\n"
"sed) or records (like awk), %s works on tree nodes.\n"
"NOTE: this program is still experimental and will probably change!\n"
"\n"
"Input\n"
"-----\n"
"\n"
"The first argument is the name of a file that contains Newick trees, or '-'\n"
"(in which case trees are read from standard input).\n"
"\n"
"The other arguments depend on whether the program was called in the first\n"
"or second form (see 'Synopsis' above).\n"
"\n"
"First Form\n"
"..........\n"
"\n"
"The second argument (selector) is a Lua expression. It is evaluated on each\n"
"node in turn. If it evaluates to true, then (and only then) the second\n"
"argument (the action) is evaluated. The selector is typically used to\n"
"select nodes, and the second argument is used to perform arbitrary actions.\n"
"For example, the following (selector, action) pair\n"
"\n"
"                                true 's()'\n"
"\n"
"has 'true' as selector and 's()' as action. This selector is always true,\n"
"so the action s() (print out the subtree rooted at the current node) is\n"
"performed for every node.\n"
"\n"
"Second Form\n"
"...........\n"
"\n"
"In this form, the Lua code is supplied in a file whose name is the\n"
"argument to option -f. The file should define \"hooks\", i.e. functions\n"
"with predetermined names which will be called at predetermined events\n"
"during the run. Here are the hook names and events:\n"
"\n"
"               Hook name      When Called\n"
"               --------------------------\n"
"               start_run      before processing any tree\n"
"               start_tree     for each tree, before processing\n"
"               node           for each node\n"
"               stop_tree      for each tree, after processing\n"
"               stop_run       after processing every tree\n"
"\n"
"You do not need to define all hooks. Any hook left undefined will be\n"
"replaced by a no-op function, and nothing will happen on the corresponding\n"
"event.\n"
"\n"
"The program provides specalized Lua functions and variables for working\n"
"with nodes (see 'Selector' and 'Action' below; they are available in hooks\n"
"as well as for command-line expressions). \n"
"\n"
"Output\n"
"------\n"
"\n"
"By default, the program prints the input tree (which may have been\n"
"modified). However, the action can cause arbitrary material to be\n"
"printed out.\n"
"\n"
"Selector\n"
"---------\n"
"\n"
"The selector expression involves node properties such as depth, bootstrap\n"
"support, whether or not a node is a leaf, etc. These are represented by\n"
"predefined variables which are set for the current node. For example, the\n"
"predefined variable 'i' is true iff the current node is internal, while\n"
"variable 'b' is set to the current node's bootstrap support value\n"
"(assuming the the node has a label and that it is numeric).\n"
"For example,\n"
"\n"
"				 i and (b > 75)\n"
"\n"
"will match internal nodes with a bootstrap support value greater than 75\n"
"(assuming that the support value is defined).\n"
"\n"
"The variables have short names to allow for compact expressions on the\n"
"command line.\n"
"\n"
"The predefined varables are:\n"
"\n"
"	Name	Lua Type	Meaning\n"
"	---------------------------------------------------------\n"
"    	a	integer		number of ancestors\n"
"    	b	number		support value\n"
"    	c	integer		number of children\n"
"    	d	number		depth (distance to root)\n"
"    	D	integer		number of descendants\n"
"    	i	boolean    	true iff node is strictly internal\n"
"    	L	number	    	parent edge's length\n"
"    	l	boolean    	true iff node is a leaf\n"
"    	lbl	string    	label\n"
"	N	node		the current node\n"
"    	r	boolean    	true iff node is the root\n"
"\n"
"Notes:\n"
"    	o Exactly one of i, l, and r is true for every node.\n"
"    	o The difference between b and lbl is that b interprets the label as\n"
"	  a number (if possible), while for lbl it is a string.\n"
"    	o Variables b, d, L and lbl may be undefined (nil).\n"
"    	o N is a Lua metatable that represents a node (see 'Nodes' below).\n"
"\n"
"Action\n"
"------\n"
"\n"
"The action can be any valid Lua expression. The program defines the\n"
"following functions that work with nodes. They all take the current node\n"
"as an implicit argument, and return nothing. Like variables, they have\n"
"short names to allow for concise expressions on the command line.\n"
"\n"
"	Name	Meaning\n"
"	---------------------------------------------------------\n"
"    	o 	'opens' a node - unlinks the node, and \n"
"			attaches children to the unliked node's parent\n"
"    	s 	prints the subtree rooted ad the current node\n"
"    	u 	unlinks the current node\n"
"\n"
"Function 'o' ('open') is useful for discarding poorly-supported nodes (see\n"
"'Examples' below).\n"
"\n"
"Nodes\n"
".....\n"
"\n"
"In addition to the basic types, some functions can work on nodes (which\n"
"are represented by Lua metatables). The predefined variable N is of that\n"
"type, and some functions also return nodes. Nodes support accessors, and\n"
"are used in the usual Lua table syntax, e.g. N['lbl'] or N.lbl.\n"
"The accessors are:\n"
"\n"
"	Name			Arg type	Meaning\n"
"	---------------------------------------------------------\n"
"    	lbl			string		label\n"
"    	b			number		support value\n"
"    	len,L			number		parent edge length\n"
"    	par			node		parent node\n"
"    	first_child, fc		node		first child\n"
"    	last_child, lc		node		last child\n"
"    	children_count, c	node		last child\n"
"    	kids			table		node's children\n"
"\n"
"Note: the label and edge length of a node can also be set, by using\n"
"the accessor in a lvalue, e.g. N.len = 0.12.\n"
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
"$ %s data/HRV.bs.nw 'i and (b <= 10)' 'o()'\n"
"\n"
"# \"open\" all nodes with bootstrap support < 750, then discard leaves that\n"
"# are directly attached to the ingroup's root. This effectively keeps only\n"
"# leaves that are part of well-supported clades.\n"
"\n"
"$ %s data/big.rn.nw 'i and (b < 750)' 'o()' | %s - 'l and (a == 2)' 'u()'\n"
"\n"
"# get all clades with at least one ancestor, 980 or better support. Do not\n"
"# print subtrees of matching clades, even if they match (option -o)\n"
"\n"
"$ %s data/big.rn.nw -n -o '(a >= 1) and (b ~= nil) and (b >= 980)' 's()'\n"
"\n"
"# format all defined lengths to 2 decimal places, instead of 6 in the\n"
"# original:\n"
"\n"
"$ %s data/HRV.nw 'L ~= nil' 'N.len = string.format(\"%%.2f\", L)'\n"
"\n"
"# In the last example, I check that L (the length) is defined\n"
"# and when true, I use N.len to set the length to a new value, namely\n"
"# the result of the call to format.\n", 
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	);
}

static void print_usage(const char *progname) 
{
	fprintf(stderr, "Usage: %s [-hnor] <filename|-> <condition> <action>\n"
			" or:   %s -f scriptname [-hnor] <filename|->\n",
			progname, progname);
}

static struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	params.lua_condition = NULL;
	params.lua_action = NULL;
	params.user_hooks_filename = NULL;
	params.show_tree = true;
	params.order = POST_ORDER;
	params.stop_clade_at_first_match = false;
	params.single = true;

	int opt_char;
	while ((opt_char = getopt(argc, argv, "f:hnor")) != -1) {
		switch (opt_char) {
		case 'f':
			params.user_hooks_filename = optarg;
			params.single = false; // TODO: still needed?
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
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

	/* check arguments */ // TODO: refactor
	if (3 == (argc - optind))	{
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
		params.lua_condition = argv[optind+1];
		params.lua_action = argv[optind+2];
	} else if (1 == (argc - optind)) {
		/* use only with -f: check */
		if (NULL == params.user_hooks_filename) {
			print_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
	} else {
		print_usage(argv[0]);
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

static int lua_set_current_node(lua_State *L, struct rnode *current)
{
	struct lua_rnode *lua_current = lua_newuserdata(L,
			sizeof (struct lua_rnode));

	luaL_getmetatable(L, "LRnode");
	lua_setmetatable(L, -2);

	// TODO: shouldn't we check for NULL?
	lua_current->orig = current;
	return 1;
}


/* Sets the value of the predefined variables (i, l, a, etc), according to the
 * node passed as argument. This is normally the current node, while visiting
 * the tree. Using variables rather than functions makes for shorter Scheme
 * expression can be shorter, because function calls can be replaced by
 * variables, e.g. (< 2 a) instead of (< 2 (a)) to check that the current node
 * has fewer than two ancestors. On the command line, this is handy. */

static void set_predefined_variables(struct rnode *node, lua_State *L)
{
	int initial_lua_stack_size = lua_gettop(L);

	/* N: current node */
	lua_set_current_node(L, node);
	lua_setglobal(L, "N");

	lua_pushstring(L, node->label);
	lua_setglobal(L, "lbl");

	/* b: returns node label, as a bootstrap support value (or nil if
	this can't be done) */
	if (is_leaf(node)) {
		lua_pushnil(L);
	} else {
		lua_pushstring(L, node->label);
		if (lua_isnumber(L, -1)) {
			lua_Number b = lua_tonumber(L, -1);
			lua_pop(L, 1);
			lua_pushnumber(L, b);
		} else {
			lua_pop(L, 1);
			lua_pushnil(L);
		}
	}
	lua_setglobal(L, "b");

	/* i: true IFF node is inner (not leaf, not root) */
	if (is_inner_node(node))
		lua_pushboolean(L, true);
	else
		lua_pushboolean(L, false);
	lua_setglobal(L, "i");

	/* L: parent edge's length */
	if (strcmp ("", node->edge_length_as_string) == 0) {
		lua_pushnil(L);
	} else {
		/* edge_length_as_string represents a number */
		lua_pushstring(L, node->edge_length_as_string);
		lua_Number len = lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pushnumber(L, len);
	}
	lua_setglobal(L, "L");

	/* l: true IFF node is a leaf */
	if (is_leaf(node))
		lua_pushboolean(L, true);
	else
		lua_pushboolean(L, false);
	lua_setglobal(L, "l");

	/* c: number of children */
	lua_pushinteger(L, (lua_Integer) node->child_count);
	lua_setglobal(L, "c");
	
	struct rnode_data *data = node->data;

	/* d: depth */
	if (data->is_depth_defined) 
		lua_pushnumber(L, data->depth);
	else
		lua_pushnil(L);
	lua_setglobal(L, "d");

	/* a: number of ancestors */
	lua_pushinteger(L, (lua_Integer) data->nb_ancestors);
	lua_setglobal(L, "a");

	/* D: number of descendants */
	//scm_c_define("D", scm_from_int(data->nb_descendants));
	lua_pushinteger(L, (lua_Integer) data->nb_descendants);
	lua_setglobal(L, "D");

	/* r: true iff node is root */
	if (is_root(node))
		lua_pushboolean(L, true);
	else
		lua_pushboolean(L, false);
	lua_setglobal(L, "r");

	/* sanity check */
	assert(lua_gettop(L) == initial_lua_stack_size);
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
		struct rnode *current_node = (struct rnode *) el->data;

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
		lua_getfield(L, LUA_GLOBALSINDEX, NODE);
		lua_call(L, 0, 0);
	} /* loop over all nodes */

	/* If order is PRE_ORDER, the list of nodes is an inverted copy, so we
	 * need to free it. */
	if (PRE_ORDER == params.order)
		destroy_llist(nodes);
}


/* Checks that 1st arg passed to a Lua function is an lnode. */

static struct lua_rnode *check_lnode(lua_State *L)
{
	void *ud = luaL_checkudata(L, 1, "LRnode");
	luaL_argcheck(L, NULL != ud, 1, "expected node");
	return (struct lua_rnode *) ud;
}

/* Returns the node on which to work. This is for functions that use the
 * current node (N) as an implicit argument IFF no argument is passed. */

static struct lua_rnode *get_lua_rnode_arg(lua_State *L)
{
	struct lua_rnode *lnode = NULL;
	int num_args = lua_gettop(L);
	switch (num_args) {
	case 0:
		lua_getglobal(L, "N");
		lnode = lua_touserdata(L, -1);
		if (NULL == lnode)
			luaL_error(L, "N is not a node");
		break;
	case 1:
		lnode = check_lnode(L);
		break;
	default:
		luaL_error(L, "too many arguments to s()");
	}

	return lnode;
}

static int lua_print_subclade(lua_State * L)
{
	struct lua_rnode *lnode = get_lua_rnode_arg(L);
	dump_newick(lnode->orig);
	return 0;
}

static int lua_open_node(lua_State *L)
{
	struct lua_rnode *lnode = get_lua_rnode_arg(L);
	struct rnode *node = lnode->orig;
	// TODO: unify error handling (see below)
	if (is_inner_node(node)) {
		if (! splice_out_rnode(node)) {
			perror("Memory error - node not spliced out.");
		}
	} else {
		fprintf (stderr, "Warning: tried to splice out non-inner node ('%s')\n", node->label);
	}

	return 0;
}

static int lua_unlink_node (lua_State * L)
{
	// int num_args = lua_gettop(L);
	struct lua_rnode *lnode = get_lua_rnode_arg(L);
	struct rnode *node = lnode->orig;

	if (is_root(node)) {
		// TODO: unify error handling (Lua? C?)
		fprintf (stderr, "Warning: tried to delete root\n");

	}
	enum unlink_rnode_status result = unlink_rnode(node);
	switch(result) {
	case UNLINK_RNODE_DONE:
	case UNLINK_RNODE_ROOT_CHILD:
		break;
	case UNLINK_RNODE_ERROR:
		fprintf (stderr, "Memory error - unlink aborted.\n");
		break;
	default:
		assert(0); /* programmer error */
	}

	return 0;
}

static void load_lua_condition(lua_State * L, char *lua_condition)
{
	char *lua_condition_chunk = masprintf("return (%s)",
					      lua_condition);
	int error = luaL_loadbuffer(L, lua_condition_chunk,
				    strlen(lua_condition_chunk),
				    CONDITION);
	free(lua_condition_chunk);
	if (error) {
		const char *msg = lua_tostring(L, -1);
		lua_pop(L, 1);
		printf("%s\n", msg);
		exit(EXIT_FAILURE);
	}
	lua_setfield(L, LUA_GLOBALSINDEX, CONDITION);
}

static void load_lua_action(lua_State *L, char *lua_action)
{
	int error = luaL_loadbuffer(L, lua_action, strlen(lua_action), ACTION);
	if (error) {
		const char *msg = lua_tostring(L, -1);
		lua_pop(L, 1);
		printf("%s\n", msg);
		exit(EXIT_FAILURE);
	}
	lua_setfield(L, LUA_GLOBALSINDEX, ACTION);
}

static void run_user_hooks_file(lua_State * L, char *user_hooks_filename)
{
	int error = luaL_dofile(L, user_hooks_filename);
	if (error) {
		const char *err_msg = lua_tostring(L, -1);
		fprintf(stderr, "%s\n", err_msg);
		exit(EXIT_FAILURE);
	}
}

static enum node_field field_string2code (const char *fld_str)
{
	if (strcmp("lbl", fld_str) == 0) return NODE_LABEL;
	if (strcmp("b", fld_str) == 0) return NODE_SUPPORT;
	if (strcmp("len", fld_str) == 0) return NODE_LENGTH;
	if (strcmp("L", fld_str) == 0) return NODE_LENGTH;
	if (strcmp("par", fld_str) == 0) return NODE_PARENT;
	if (strcmp("fc", fld_str) == 0) return NODE_FIRST_CHILD;
	if (strcmp("first_child", fld_str) == 0) return NODE_FIRST_CHILD;
	if (strcmp("lc", fld_str) == 0) return NODE_LAST_CHILD;
	if (strcmp("last_child", fld_str) == 0) return NODE_LAST_CHILD;
	if (strcmp("c", fld_str) == 0) return NODE_CHILD_COUNT;
	if (strcmp("children_count", fld_str) == 0) return NODE_CHILD_COUNT;
	if (strcmp("kids", fld_str) == 0) return NODE_CHILDREN;

	return UNKNOWN_FIELD;
}

static int lua_node_set(lua_State *L)
{
	struct lua_rnode *lnode = check_lnode(L);
	const char *field = luaL_checkstring(L, 2);
	luaL_argcheck(L, NULL != field, 2, "expected string");
	enum node_field field_code = field_string2code(field);

	switch (field_code) {
	case NODE_LABEL:
		luaL_argcheck(L, lua_isstring(L, 3), 3, "expected a string");
		const char *label = lua_tostring(L, 3);
		free(lnode->orig->label);
		lnode->orig->label = strdup(label);
		return 0;
	case NODE_LENGTH:
		if (lua_isnumber(L, 3)) {
			const char *len_s = lua_tostring(L, 3);
			free(lnode->orig->edge_length_as_string);
			lnode->orig->edge_length_as_string = strdup(len_s);
			return 0;
		} else if (lua_isstring(L, 3)) {
			/* already checked for numbers, so this is a
			 * "non-numeric" string */
			const char *len_s = lua_tostring(L, 3);
			luaL_argcheck(L, '\0' == *len_s, 3,
				"expected a number, a number-convertible "
				"string, or the empty string.");
			free(lnode->orig->edge_length_as_string);
			lnode->orig->edge_length_as_string = strdup("");
			return 0;
		} else {
			luaL_error(L, false, 3,
				"expected a number, a number-convertible "
				"string, or the empty string.");
		}
	case UNKNOWN_FIELD:
		lua_pushnil(L);
		return luaL_error(L, "error: '%s' is not a node field.", field);
	default:
		assert(0); /* programmer error */
	}

	return 0;
}

static void push_new_lnode(lua_State *L, struct rnode *orig)
{
	struct lua_rnode *lua_result = lua_newuserdata(
			L, sizeof (struct lua_rnode));
	luaL_getmetatable(L, "LRnode");
	lua_setmetatable(L, -2);
	// TODO: shouldn't we check for NULL?
	lua_result->orig = orig;
}

static void push_kids(lua_State *L, struct rnode *orig)
{
	struct rnode *current_kid = orig->first_child;
	lua_createtable(L, orig->child_count, 0);
	int index = 0;
	for(; NULL != current_kid; current_kid = current_kid->next_sibling) {
		index++;
		lua_pushinteger(L, index);
		push_new_lnode(L, current_kid); 
		/* now we have the table at -3, the key (index) at -2  and the
		 * value on top (-1) */
		lua_settable(L, -3);
	}
}

static int lua_node_get(lua_State *L)
{
	struct lua_rnode *lnode = check_lnode(L);
	const char *field = luaL_checkstring(L, 2);
	luaL_argcheck(L, NULL != field, 2, "expected string");
	enum node_field field_code = field_string2code(field);
	//struct lua_rnode *lua_result = NULL;
	struct rnode *orig = lnode->orig;

	switch (field_code) {
	case NODE_LABEL:
		lua_pushstring(L, orig->label);
		return 1;
	case NODE_LENGTH:
		lua_pushnumber(L, atof(orig->edge_length_as_string));
		return 1;
	case NODE_PARENT:
		push_new_lnode(L, orig->parent);
		return 1;
	case NODE_FIRST_CHILD:
		push_new_lnode(L, orig->first_child);
		return 1;
	case NODE_LAST_CHILD:
		push_new_lnode(L, orig->last_child);
		return 1;
	case NODE_CHILD_COUNT:
		lua_pushinteger(L, orig->child_count);
		return 1;
	case NODE_CHILDREN:
		push_kids(L, orig);
		return 1;
	case UNKNOWN_FIELD:
		lua_pushnil(L);
		return luaL_error(L, "error: '%s' is not a node field.", field);
	default:
		assert(0); /* programmer error */
	}
}

/* does nothing - uses this one when passed -f and the use has not written a
 * node() function. */

static int lua_no_op(lua_State *L) { return 0; }

/* Processes the current node according to the condtion and action given on the
 * command line. */

static int lua_cli_process_node(lua_State *L)
{
	lua_getglobal(L, "N");
	struct lua_rnode *lnode = lua_touserdata(L, -1);
	if (NULL == lnode)
		luaL_error(L, "N is not a node");

	lua_getfield(L, LUA_GLOBALSINDEX, CONDITION);
	lua_call(L, 0, 1);
	if (lua_isboolean(L, -1) != 1) {
		fprintf(stderr, "WARNING: condition does not evaluate "
				"to a boolean.\n");
		lua_pop(L, 1);
		return 0;
	}
	int match = lua_toboolean(L, -1);
	lua_pop(L, 1);
	if (match) {
		lua_getfield(L, LUA_GLOBALSINDEX, ACTION);
		lua_call(L, 0, 0);
		/* see -o switch */
		lua_getfield(L, LUA_GLOBALSINDEX, STOP_AT_1ST_MATCH);
		int stop_clade_at_first_match = lua_toboolean(L, -1);
		lua_pop(L, 1);
		if (stop_clade_at_first_match)
			((struct rnode_data *) lnode->orig->data)->stop_mark = true;
	}
	return 0;
}

// TODO: implement __tostring for lua_rnode

/* Functions for Lua node */
static const struct luaL_reg lnodelib_f [] = {
	{"set", lua_node_set},
	{"get", lua_node_get},
	{NULL, NULL}
};

/* Cf http://www.lua.org/pil/28.3.html */

static int luaopen_lnode (lua_State *L) 
{
	luaL_newmetatable(L, "LRnode");
	luaL_openlib(L, "lnode", lnodelib_f, 0);

	lua_pushstring(L, "__index");
	lua_pushstring(L, "get");
	lua_gettable(L, 2);	/* get lnode.get */
	lua_settable(L, 1);	/* mt.__index = lnode.get */

	lua_pushstring(L, "__newindex");
	lua_pushstring(L, "set");
	lua_gettable(L, 2);	/* get lnode.set */
	lua_settable(L, 1);	/* mt.__index = lnode.set */

	return 0;
}

static void run_user_hook(lua_State *L, const char *phase)
{
	lua_getfield(L, LUA_GLOBALSINDEX, phase);
	// TODO: this lua_isfunction() check should be done once and for all,
	// before the tree is processed, like for the NODE function (see main(),
	// around l. 956
	if (lua_isfunction(L, -1))
		lua_call(L, 0, 0);
	else 
		lua_pop(L, 1);
}

int main(int argc, char* argv[])
{
	struct parameters params = get_params(argc, argv);
	struct rooted_tree *tree;

	/* Initializes Lua */
	lua_State *L = lua_open();   
	luaL_openlibs(L);

	lua_pushcfunction(L, lua_print_subclade);
	lua_setglobal(L, "s");
	lua_pushcfunction(L, lua_unlink_node);
	lua_setglobal(L, "u");
	lua_pushcfunction(L, lua_open_node);
	lua_setglobal(L, "o");
	lua_pushboolean(L, params.stop_clade_at_first_match);
	lua_setglobal(L, STOP_AT_1ST_MATCH);


	luaopen_lnode(L);

	if (NULL != params.user_hooks_filename) {
		run_user_hooks_file(L, params.user_hooks_filename);
		lua_getfield(L, LUA_GLOBALSINDEX, NODE);
		int NODE_type = lua_type(L, -1);
		switch(NODE_type) {
		case LUA_TFUNCTION:
			break; /* ok, do nothing */
		case LUA_TNIL:
			/* function node() was not defined: set it to no-op */
			lua_pushcfunction(L, lua_no_op);
			lua_setglobal(L, NODE);
			break;
		default:
			fprintf(stderr, "WARNING: 'node' is not a function"
				" - substituting no-op.\n");
			lua_pushcfunction(L, lua_no_op);
			lua_setglobal(L, NODE);
		}
	} else {
		lua_pushcfunction(L, lua_cli_process_node);
		lua_setglobal(L, NODE);
		load_lua_condition(L, params.lua_condition);
		load_lua_action(L, params.lua_action);
	}


	run_user_hook(L, START);
	while (NULL != (tree = parse_tree())) {
		run_user_hook(L, START_TREE);
		process_tree(tree, L, params);
		if (params.show_tree) {
			dump_newick(tree->root);
		}
		run_user_hook(L, STOP_TREE);
		destroy_all_rnodes(NULL);
		destroy_tree(tree);
	}
	run_user_hook(L, STOP);

	exit(EXIT_SUCCESS);
}
