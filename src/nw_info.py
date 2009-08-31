#!/usr/bin/env python

from newick_utils import *

for tree in Tree.parse_newick_input():
	for node in tree.get_nodes():
		print node.get_label()
