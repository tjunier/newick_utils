#!/usr/bin/env python

# A (simple) Python version of nw_reroot. Accepts only a single label.

import sys
from newick_utils import *

def main():

	if len(sys.argv) < 3:
		raise RuntimeError ("Usage: nw_reroot.py <-|filename> <label>")

	filename = ''
	if sys.argv[1] != '-':
		filename = sys.argv[1]

	label = sys.argv[2]

# "Thin" interface: mostly direct calls to libnw functions.

	if filename != '':
		libnw.set_parser_input_filename(filename)
	tree = libnw.parse_tree()
	lbl2node = libnw.create_label2node_map(tree.contents.nodes_in_order)
	node = libnw.hash_get(lbl2node, label)
	node = cast(node, POINTER(rnode))
	libnw.reroot_tree(tree, node)
	libnw.dump_newick(tree.contents.root)

#   The following does the same thing, but with a more object-oriented
#   interface. Use it if you need to manipulate the tree from Python.
#	for tree in Tree.parse_newick_input(filename):
#		outgroup = tree.get_node_with_label(label)
#		tree.reroot(outgroup)
#		tree.dump_newick()

if __name__ == "__main__":
	main()
