#!/usr/bin/env python

from newick_utils import *

def count_polytomies(tree):
	count = 0
	for node in tree.get_nodes():
		if node.children_count() > 2:
			count += 1
	return count

for tree in Tree.parse_newick_input():
	type = tree.get_type()
	if type == 'Phylogram':
		# also sets nodes' depths
		depth =  tree.get_depth() 
	else:
		depth = None
	print 'Type:', type
	print '#Nodes:', len(list(tree.get_nodes()))
	print '  #leaves:', tree.get_leaf_count()
	print '#Polytomies:', count_polytomies(tree)
	print "Depth:", depth
