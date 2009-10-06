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

	for tree in Tree.parse_newick_input(filename):
		outgroup = tree.get_node_with_label(label)
		tree.reroot(outgroup)
		print tree.to_newick()

if __name__ == "__main__":
	main()
