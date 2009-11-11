#!/usr/bin/env python

import sys
from newick_utils import *

def count_polytomies(tree):
	count = 0
	for node in tree.get_nodes():
		if node.children_count() > 2:
			count += 1
	return count

# Main

def main():

	if len(sys.argv) < 2:
		raise RuntimeError ("Usage: toy.py <-|filename>")

	filename = ''
	if sys.argv[1] != '-':
		filename = sys.argv[1]

	input = '(A,(B,C));((Drosophila:1,Tribolium:1.2,Vespa:0.23):1,(Daphnia:2.0,Homarus:1.3):3.1);'
	for tree in Tree.parse_newick_input(input, type='string'):
		type = tree.get_type()
		if type == Tree.PHYLOGRAM:
			# also sets nodes' depths
			depth =  tree.get_depth() 
		else:
			depth = None
		print 'Type:', type
		print '#Nodes:', len(list(tree.get_nodes()))
		print '  #leaves:', tree.get_leaf_count()
		print '#Polytomies:', count_polytomies(tree)
		print "Depth:", depth

if __name__ == "__main__":
	main()
