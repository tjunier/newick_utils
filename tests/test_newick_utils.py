#!/usr/bin/env python

import unittest, sys

sys.path.append('../src')

from newick_utils import *

class TestHash (unittest.TestCase):

	def setUp(self):
		c_hash = libnw.create_hash(10)
		self.py_hash = Hash(c_hash)

	def test_set_and_get(self):
		self.py_hash['Romeo'] = 'Juliet'
		val = cast(self.py_hash['Romeo'], c_char_p)
		self.assertEqual(val.value, 'Juliet')

	def test_none(self):
		self.assertEqual(None, self.py_hash['Caliban'])

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
		self.phylo3 = Tree.parse_newick_input(
				'(((A:2,B:1.5)f:1,C:0.5)g:2,(D:1,E:1)h:1.5)i;',
				type = 'string').next()

	def test_get_type(self):
		self.assertEqual('Cladogram', self.clado1.get_type())
		self.assertEqual('Phylogram', self.phylo1.get_type())
		self.assertEqual('Phylogram', self.phylo2.get_type())
		self.assertEqual('Neither', self.neither.get_type())

	def test_get_depth(self):
		self.assertEqual(2, self.phylo1.get_depth())

	def test_lca_from_labels(self):
		lca = self.clado2.lca_from_labels(('A', 'C'))
		self.assertEqual('g', lca.label)
		lca = self.clado2.lca_from_labels(('C'))
		self.assertEqual('C', lca.label)
		lca = self.clado2.lca_from_labels(('C', 'h' ))
		self.assertEqual('i', lca.label)

	def test_get_nodes_with_label(self):
		nodes_A = self.clado2.get_nodes_with_label('A')
		self.assertEqual(1, len(nodes_A))
		node_A = nodes_A[0]
		self.assertEqual('A', node_A.label)

	def test_get_node_with_label(self):
		node_A = self.clado2.get_node_with_label('A')
		self.assertEqual('A', node_A.label)

	def test_reroot_clado_leaf(self):
		node_A = self.clado2.get_node_with_label('A')
		self.clado2.reroot(node_A)
		post_reroot_newick = self.clado2.to_newick()
		expected = '(A,(B,(C,(D,E)h)g)f);'
		self.assertEqual(expected, post_reroot_newick)

	def test_reroot_clado_inner(self):
		node_f = self.clado2.get_node_with_label('f')
		self.clado2.reroot(node_f)
		post_reroot_newick = self.clado2.to_newick()
		expected = '((A,B)f,(C,(D,E)h)g);'
		self.assertEqual(expected, post_reroot_newick)

	def test_reroot_phylo_leaf(self):
		node_A = self.phylo3.get_node_with_label('A')
		self.phylo3.reroot(node_A)
		post_reroot_newick = self.phylo3.to_newick()
		expected = '(A:1,(B:1.5,(C:0.5,(D:1,E:1)h:3.5)g:1)f:1);'
		self.assertEqual(expected, post_reroot_newick)

	def test_reroot_phylo_inner(self):
		node_f = self.phylo3.get_node_with_label('f')
		self.phylo3.reroot(node_f)
		post_reroot_newick = self.phylo3.to_newick()
		expected = '((A:2,B:1.5)f:0.5,(C:0.5,(D:1,E:1)h:3.5)g:0.5);'
		self.assertEqual(expected, post_reroot_newick)


if __name__ == '__main__':
	unittest.main()
