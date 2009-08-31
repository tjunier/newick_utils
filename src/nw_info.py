#!/usr/bin/env python

from newick_utils import *

for tree in Tree.parse_newick_input():
	for node in tree.nodes():
		print node.label(), node.is_leaf()
