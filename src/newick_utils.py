# Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
# * Neither the name of the University of Geneva nor the names of its
#     contributors may be used to endorse or promote products derived from this
#     software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os

from ctypes import *
from ctypes.util import find_library

################################################################
# Find and load library -- NOTE: the lib must have been installed (make
# install)

lib_dirs = ('/lib', '/lib64', '/usr/lib', '/usr/lib64', '/usr/local/lib', 
		'/usr/local/lib64')
libnw_file = find_library('nw')

libnw = None
for dir in lib_dirs:
	lib_path = os.path.join(dir, libnw_file)
	try:
		libnw = CDLL(lib_path)
		break
	except:
		pass

if libnw == None:
	raise IOError, 'libnw not found'

################################################################
# C structures

class list_elem(Structure):
	pass
list_elem._fields_ = [('next', POINTER(list_elem)),
		      ('data', c_void_p)]

class llist(Structure):
	_fields_ = [('head', POINTER(list_elem)),
		    ('tail', POINTER(list_elem)),
		    ('count', c_int)]

class hash(Structure):
	_fields_ = [
			('llist', POINTER(POINTER(llist))),
			('size', c_int),
			('count', c_int)
			]

class rnode(Structure):
	pass
rnode._fields_ = [('parent', POINTER(rnode)),
		  ('edge_length_as_string', c_char_p),
		  ('edge_length', c_double),
		  ('children', POINTER(llist)),
		  ('label', c_char_p),
		  ('data', c_void_p)]

class rooted_tree(Structure):
	_fields_ = [('root', POINTER(rnode)),
		    ('nodes_in_order', POINTER(llist))]

################################################################
# C enums mapped to Python constants

# tree.h
(TREE_TYPE_UNKNOWN, TREE_TYPE_CLADOGRAM, TREE_TYPE_PHYLOGRAM,
		TREE_TYPE_NEITHER) = xrange(4)
type_names = ('Unknown', 'Cladogram', 'Phylogram', 'Neither') 

################################################################
# C functions

libnw.create_hash.argtypes = [c_int]
libnw.create_hash.restype = POINTER(hash)
libnw.hash_set.argtypes = [POINTER(hash), c_char_p, c_void_p]
libnw.hash_get.argtypes = [POINTER(hash), c_char_p]
libnw.hash_get.restype = c_void_p

libnw.create_llist.restype = POINTER(llist)
libnw.append_element.argtypes = [POINTER(llist), c_void_p]

libnw.set_parser_input_filename.argtypes = [c_char_p]
libnw.newick_scanner_set_string_input.argtypes = [c_char_p]
libnw.parse_tree.restype = POINTER(rooted_tree)

libnw.to_newick.argtypes = [POINTER(rnode)]
libnw.to_newick.restype = c_char_p
libnw.dump_newick.argtypes = [POINTER(rnode)]
libnw.dump_newick.restype = None

libnw.is_leaf.argtypes = [POINTER(rnode)]
libnw.children_count.argtypes = [POINTER(rnode)]

libnw.lca_from_labels_multi.argtypes = [POINTER(rooted_tree), POINTER(llist)]
libnw.lca_from_labels_multi.restype = POINTER(rnode)

libnw.get_tree_type.argtypes = [POINTER(rooted_tree)]
libnw.reroot_tree.argtypes = [POINTER(rooted_tree), POINTER(rnode)]

libnw.create_label2node_map.argtypes = [POINTER(llist)]
libnw.create_label2node_map.restype = POINTER(hash)

################################################################
# User-land Python classes


class Llist(object):

	'''This class is meant to be used as an iterator, e.g.
	      list = Llist(list_pointer)
		  for element in list:
		      # do something with 'element' '''

	def __init__(self, llist):
		'''Constructor. Arg is a llist'''
		self.llist = llist
		current = self.llist.head
		self.py_list = []
		while bool(current):
			self.py_list.append(current.contents.data)
			current = current.contents.next

	def __iter__(self):
		return iter(self.py_list)

class Hash(object):

	def __init__(self, c_hash_p):
		self.hash = c_hash_p.contents

	def __setitem__(self, key, value):
		libnw.hash_set(self.hash, key, value)

	def __getitem__(self, key):
		return libnw.hash_get(self.hash, key)

class Rnode(object):

	# This dict maps C adresses to Rnode objects. This way, given the adress of
	# a C rnode structure, one can get the corresponding Rnode object. 
	c_addr_to_py_obj = {}

	def __init__(self, c_rnode_p):
		'''Constructor. Arg is a POINTER(rnode)'''
		self.rnode = c_rnode_p.contents
		c_rnode_address = cast(c_rnode_p, c_void_p).value
		# Add self to dictionary (see above)
		Rnode.c_addr_to_py_obj[c_rnode_address] = self
		self.label = self.rnode.label
		self.parent = None

	def get_parent(self):
		if self.parent is None:
			# We can't do this in the ctor since parent may not be in the
			# 'c_addr_to_py_obj' dict yet.
			parent_addr = cast(self.rnode.parent, c_void_p).value
			self.parent = Rnode.c_addr_to_py_obj[parent_addr]
		return self.parent

	def set_depth(self, depth):
		self.depth = depth

	def get_depth(self):
		if hasattr(self, 'depth'):
			return self.depth
		else:
			return None

	def get_edge_length(self):
		if not hasattr(self, 'edge_length'):
			edge_length = self.rnode.edge_length_as_string
			if edge_length == '':
				self.edge_length = None
			else:
				self.edge_length = float(edge_length)
		return self.edge_length
	
	def is_leaf(self):
		return bool(libnw.is_leaf(self.rnode))

	def children_count(self):
		return libnw.children_count(self.rnode)

class Tree(object):

	@classmethod
	def parse_newick_input(cls, source='', type='filename'):
		'''
		A generator method that yields trees.
		Usage e.g.:
			parse_newick_input()	# parses stdin
			parse_newick_input('my_file')	# parses 'my file'
			# parse a string:
			parse_newick_input('((A,B),C);', type='string')
		'''
		# Set input source
		if type == 'filename': 
			if source != '':	# a named file; otherwise uses stdin
				libnw.set_parser_input_filename(source)
		elif type == 'string':
			libnw.newick_scanner_set_string_input(source)
		else:
			raise RuntimeError("Unknown type '%s'" % type)
		# Yield trees
		while True:
			tree = libnw.parse_tree()
			if bool(tree):
				yield Tree(tree.contents)
			else:
				return

	def __init__(self, tree):
		'''Do not call this function directly. Use Tree.parse_newick_input to
		parse trees from an input source.'''
		self.tree = tree
		self.root = tree.root.contents
		nodes_in_order = Llist(self.tree.nodes_in_order.contents)
		self.depth = None
		self.label2nodes = None
		# Create an Rnode for each C rnode structure
		for data in nodes_in_order:
			c_node_p = cast(data, POINTER(rnode))
			Rnode(c_node_p)

	def dump_newick(self):
		'''Prints Newick to stdout (iterative, fast)'''
		libnw.dump_newick(self.tree.root)

	def to_newick(self):
		'''Returns a Newick representation of the tree (recursive, slow).'''
		return libnw.to_newick(self.tree.root)

	def dump_newick(self):
		'''Dumps the tree as Newick to stdout (fast)'''
		libnw.dump_newick(self.tree.root)

	def get_nodes(self):
		'''Returns an iterator over all the tree's nodes, in post-order'''
		nodes_in_order = Llist(self.tree.nodes_in_order.contents)
		for data in nodes_in_order:
			yield Rnode.c_addr_to_py_obj[data]

	def get_depth(self):
		'''Returns the tree's depth, i.e. the depth of the deepest leaf
		(in branch length units). The value is computed IFF it is None,
		otherwise it is just returned. Any alteration to the tree can make it
		invalid.
		This method also sets the Rnodes' depth attribute.'''
		if self.depth is not None:
			return self.depth
		# Compute depth, setting nodes' depth along the way
		depth_list = []
		nodelist = list(self.get_nodes())
		nodelist.reverse()
		for node in nodelist[1:]:	# skip root
			parent = node.get_parent()
			parent_depth = parent.get_depth()
			if parent_depth is None: parent_depth = 0
			node.set_depth(parent_depth + node.get_edge_length())
			depth_list.append(node.get_depth())
		self.depth = max(depth_list)
		return self.depth

	def get_ancestor_depth(self):
		'''Returns the tree's ancestor depth, i.e. the number of ancestors
		of the deepest leaf (expressed in numbers of ancestors)'''
		pass

	def get_type(self):
		'''Returns CLADOGRAM IFF tree contains NO edge lengths (this is
		different from zero-length edges!), otherwise returns PHYLOGRAM'''
		return type_names[libnw.get_tree_type(self.tree)]

	def get_leaf_count(self):
		'''Returns the number of leaves in this tree.'''
		count = 0
		for node in self.get_nodes():
			if node.is_leaf():
				count += 1
		return count

	def get_nodes_with_label(self, label):
		'''Returns a tuple of Rnode objects that have the specified label, or
		None if no such node exists.
		If you know that labels are unique, you can use get_node_with_label
		(that's 'node' without 's') which returns the node directly. '''
		if self.label2nodes is None: self.__compute_label2nodes()
		return self.label2nodes[label]

	def get_node_with_label(self, label):
		'''Returns the Rnode object that has the specified label, or None if
		no such node exists. If more than one node can have the same label, use
		get_node_with_label ('nodes' with an 's') '''
		if self.label2nodes is None: self.__compute_label2nodes()
		return self.label2nodes[label][0]

	def lca_from_labels(self, labels):
		'''Returns an Rnode which is the last common ancestor of the nodes 
		whose labels are passed in list 'labels_list', or None if for some
		reason the LCA can't be found.'''
		c_labels = libnw.create_llist()
		for label in labels:
			libnw.append_element(c_labels, label)
		c_lca = libnw.lca_from_labels_multi(self.tree, c_labels)
		c_rnode_address = cast(c_lca, c_void_p).value
		return Rnode.c_addr_to_py_obj[c_rnode_address]

	def reroot(self, node):
		'''Reroots the tree on 'node'.
		Note: this invalidates some properties of the tree (like the node
		list). The underlying C function does NOT update them, because it was
		meant to do only what is strictly necessary - in this case, reroot the
		tree and print it.  Bottom line: don't use the tree for anything else
		than printing.'''
		libnw.reroot_tree(self.tree, node.rnode)

	def __compute_label2nodes(self):
		self.label2nodes = {}
		for node in self.get_nodes():
			label = node.label
			if label not in self.label2nodes:
				self.label2nodes[label] = []
			self.label2nodes[label].append(node)
