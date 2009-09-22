#!/usr/bin/env python

import unittest, sys

sys.path.append('../src')

from newick_utils import *

class TestHash (unittest.TestCase):

	def test_set_and_get(self):
		c_hash = libnw.create_hash(10)
		py_hash = Hash(c_hash)
		py_hash['Romeo'] = 'Juliet'
		val = cast(py_hash['Romeo'], c_char_p)
		self.assertEqual(val.value, 'Juliet')

class TestTree (unittest.TestCase):

	def setUp(self):
		self.clado1 = Tree.parse_newick_input(
				'((A,B),C);', type='string').next()
		self.phylo1 = Tree.parse_newick_input(
				'((A:1,B:1):1,C:1);', type='string').next()
		self.phylo2 = Tree.parse_newick_input(
				'((A:1,B:1):1,C:1):1;', type='string').next()
		self.neither = Tree.parse_newick_input(
				'((A,B:1),C:1):1;', type='string').next()
		self.clado2 = Tree.parse_newick_input(
				'(((A,B)f,C)g,(D,E)h)i;', type = 'string').next()

	def test_get_type(self):
		self.assertEqual(TREE_TYPE_CLADOGRAM, self.clado1.get_type())
		self.assertEqual(TREE_TYPE_PHYLOGRAM, self.phylo1.get_type())
		self.assertEqual(TREE_TYPE_PHYLOGRAM, self.phylo2.get_type())
		self.assertEqual(TREE_TYPE_NEITHER, self.neither.get_type())

	def test_get_depth(self):
		self.assertEqual(2, self.phylo1.get_depth())

	def test_lca_from_labels(self):
		labels = ('A', 'C')
		lca = self.clado2.lca_from_labels(labels)
		self.assertEqual('g', lca.label)
		lca = self.clado2.lca_from_labels(('C'))
		self.assertEqual('C', lca.label)

if __name__ == '__main__':
	unittest.main()
