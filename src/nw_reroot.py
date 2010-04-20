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

#	for tree in Tree.parse_newick_input(filename):
#		outgroup = tree.get_node_with_label(label)
#		tree.reroot(outgroup)
#		tree.dump_newick()

	tree = libnw.parse_tree()
	lbl2node = libnw.create_label2node_map(tree.contents.nodes_in_order)
	node = libnw.hash_get(lbl2node, "Homo")
	node = cast(node, POINTER(rnode))
	print node.contents.label

if __name__ == "__main__":
	main()
