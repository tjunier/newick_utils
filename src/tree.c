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
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>

#include "tree.h"
#include "link.h"
#include "rnode.h"
#include "list.h"
#include "nodemap.h"
#include "hash.h"
#include "rnode_iterator.h"
#include "to_newick.h" // TODO: rm when debugged

const int FREE_NODE_DATA = 1;
const int DONT_FREE_NODE_DATA = 0;

/* 'outgroup' is the node which will be the outgroup after rerooting. */

void reroot_tree(struct rooted_tree *tree, struct rnode *outgroup)
{
	struct rnode *old_root = tree->root;
	struct rnode *new_root;
	struct llist *revert_list;
	struct rnode *node;
	struct list_elem *elem;

	/* Insert node (will be the new root) above outgroup */
	insert_node_above(outgroup, "NEW");
	new_root = outgroup->parent;

	/* Invert edges from old root to new root (i.e., the tree is always in
	 * a consistent state) */

	/* First, we make a list of the edges we need to revert, by visiting
	 * the tree from the soon-to-be new root to the old (which is still the
	 * root) */
	revert_list = create_llist();
	for (node = new_root; ! is_root(node); node = node->parent) {
		/* order of reversals is important: tree is always consistent */
		prepend_element(revert_list, node);
	}
	/* Now, we reverse the edges in the list. */ // TODO: may bot be needed w/o redges
	for (elem = revert_list->head; NULL != elem; elem = elem->next) {
		struct redge *edge = (struct redge*) elem->data;
		// reverse_redge(edge);
	}
        destroy_llist(revert_list);

	if (children_count(old_root) == 1) {
		splice_out_rnode(old_root);
		// fprintf (stderr, "freeing '%s' (spliced out).\n", old_root->label);
		destroy_rnode(old_root, NULL);
	}

	tree->root = new_root;
	/* TODO: The tree's structure was changed, so 'nodes_in_order' is now
	 * invalid. when get_nodes_in_order() is fixed, we shall use it here.
	 * For now we annull it so it cannot be used. */
        destroy_llist(tree->nodes_in_order);
	tree->nodes_in_order = NULL;
}

/* Returns true IFF all children are leaves. Assumes n is not a leaf. */

int all_children_are_leaves(struct rnode *n)
{
	struct list_elem *el;
	for (el = n->children->head; NULL != el; el = el->next) {
		struct rnode *child = el->data;
		if (! is_leaf(child)) return 0;
	}

	return 1;
}

/* Returns true IFF all children have the same label. If true, sets 'label' to
 * the shared label. Assumes n is inner node, and all its children are leaves. */

int all_children_have_same_label(struct rnode *n, char **label)
{

	/* get first child's label */

	struct list_elem *el = n->children->head;
	struct rnode *child = el->data;
	char *ref_label = child->label;

	/* iterate over other children, and compare their label to the first's */

	for (el = el->next; NULL != el; el = el->next) {
		child = el->data;
		if (0 != strcmp(ref_label, child->label))
			return 0; /* found a different label */
	}

	*label = ref_label;
	return 1;
}

void collapse_pure_clades(struct rooted_tree *tree)
{
	struct list_elem *el;		

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		if (is_leaf(current)) continue;	/* can only collapse inner nodes */
		/* attempt collapse only if all children are leaves (any pure
		 * subtree will have been collapsed to a leaf by now) */
		if (! all_children_are_leaves(current)) continue;
		char *label;
		if (all_children_have_same_label(current, &label)) {
			/* set own label to children's label  - we copy it
			 * because it will be later passed to free() */
			free(current->label);
			current->label = strdup(label);
			/* remove children */
			clear_llist(current->children);
		}
	}
}

void destroy_tree(struct rooted_tree *tree, int free_node_data)
{
	struct list_elem *e;

	/* Traversing in post-order ensures that children list's data are
	 * already empty when we destroy the list (since the lists contain
	 * children edges) */
	for (e = tree->nodes_in_order->head; NULL != e; e = e->next) {
		struct rnode *current = e->data;
		destroy_llist(current->children);
		free(current->label);
		free(current->edge_length_as_string);
		/* only works if data can be free()d, i.e. has no pointer to
		 * allocated storage. Otherwise free the data "manually". */
		if (free_node_data)
			free(current->data);
		free(current);
	}

	destroy_llist(tree->nodes_in_order);
	free(tree);
}

int leaf_count(struct rooted_tree * tree)
{
	struct list_elem *el;
	int n = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		if (is_leaf((struct rnode *) el->data)) {
			n++;
		}
	}

	return n;
}

struct llist *get_leaf_labels(struct rooted_tree *tree)
{
	struct llist *labels = create_llist();
	struct list_elem *el;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (is_leaf(current)) 
			if (strcmp ("", current->label) != 0)
				append_element(labels, current->label);
	}

	return labels;
}

struct llist *get_labels(struct rooted_tree *tree)
{
	struct llist *labels = create_llist();
	struct list_elem *el;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = (struct rnode *) el->data;
		if (strcmp ("", current->label) != 0)
			append_element(labels, current->label);
	}

	return labels;
}

/* TODO: this could be derived by the parser and stored in a member of the
 * rooted_tree structure. */

int is_cladogram(struct rooted_tree *tree)
{
	struct list_elem *el;
	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *current = el->data;
		if (strcmp(current->edge_length_as_string, "") != 0)
			return 0;
	}

	return 1;
}

struct llist *nodes_from_labels(struct rooted_tree *tree,
		struct llist *labels)
{
	struct hash *label2node_map = create_label2node_map(
			tree->nodes_in_order); 
	struct llist *result = create_llist();
	struct list_elem *el;
	for (el = labels->head; NULL != el; el = el->next) {
		char *label = el->data;
		struct rnode *node = hash_get(label2node_map, label);
		if (NULL == node) {
			fprintf (stderr, "WARNING: label '%s' not found.\n",
					label);
		} else {
			append_element(result, node);
		}
	}
	destroy_hash(label2node_map);

	return result;
}

struct llist *nodes_from_regexp(struct rooted_tree *tree,
		char *regexp_string)
{
	int errcode;
	regex_t *preg = malloc(sizeof(regex_t));
	int cflags = 0;
	if (NULL == preg) {perror(NULL); exit(EXIT_FAILURE);}
	errcode = regcomp(preg, regexp_string, cflags);
	if (errcode) {
		size_t errbufsize = regerror(errcode, preg, NULL, 0);
		char *errbuf = malloc(errbufsize * sizeof(char));
		if (NULL == errbuf) {perror(NULL); exit(EXIT_FAILURE);}
		regerror(errcode, preg, errbuf, errbufsize);
		fprintf (stderr, "%s\n", errbuf);
		exit(EXIT_FAILURE);
	}
       				       
	struct llist *result = create_llist();
	struct list_elem *el;

	size_t nmatch = 1;	/* either matches or doesn't */
	regmatch_t pmatch[nmatch]; 
	int eflags = 0;

	for (el = tree->nodes_in_order->head; NULL != el; el = el->next) {
		struct rnode *node = el->data;
		errcode = regexec(preg, node->label, nmatch, pmatch, eflags);	
		if (0 == errcode) {
			append_element(result, node);
		}
	}
	/* This does not free 'preg' itself, only memory pointed to by 'preg'
	 * members and allocated by regcomp().*/
	regfree(preg);
	/* Therefore: */
	free(preg);

	return result;
}

/* Clones a clade (recursively) */
// TODO: try an iterative version using a rnode_iterator
static struct rnode *clone_clade(struct rnode *root)
{
	struct rnode *root_clone = create_rnode(root->label,"");
	struct list_elem *el;
	for (el = root->children->head; NULL != el; el = el->next) {
		struct rnode *kid = el->data;
		struct rnode *kid_clone = clone_clade(kid);
		add_child(root_clone, kid_clone); // TODO: handle length?
	}

	return root_clone;
}

struct rooted_tree* clone_subtree(struct rnode *root)
{
	struct rnode *root_clone = clone_clade(root);
	struct llist *nodes_in_order_clone = get_nodes_in_order(root_clone);

	struct rooted_tree *clone = malloc(sizeof(struct rooted_tree));
	if (NULL == clone) { perror(NULL); exit(EXIT_FAILURE); }

	clone->root = root_clone;
	clone->nodes_in_order = nodes_in_order_clone;

	return clone;
}
