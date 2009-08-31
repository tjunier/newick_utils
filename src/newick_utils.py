import os

from ctypes import *
from ctypes.util import find_library

################################################################
# Find and load library

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
# C functions

libnw.parse_tree.restype = POINTER(rooted_tree)

libnw.to_newick.argtypes = [POINTER(rnode)]
libnw.to_newick.restype = c_char_p

libnw.is_leaf.argtypes = [POINTER(rnode)]

################################################################
# User-land Python classes

class Llist(object):

	def __init__(self, llist):
		self.llist = llist
		self.current = llist.head.contents

	def __iter__(self):
		return self

	def next(self):
		try:
			result = self.current.data
			next = self.current.next.contents
			self.current = next
			return result
		except ValueError:
			raise StopIteration
		
class Rnode(object):

	def __init__(self, rnode):
		'''Constructor. Arg is an r'''
		self.rnode = rnode

	def get_label(self):
		return self.rnode.label

	def is_leaf(self):
		return bool(libnw.is_leaf(pointer(self.rnode)))

	def get_edge_length(self):
		if not hasattr(self, 'edge_length'):
			self.edge_length = float(self.rnode.edge_length_as_string)
		return self.edge_length

class Tree(object):

	@classmethod
	def parse_newick_input(cls):
		'''A generator method that yields trees. E.g. for tree in parse_newick_input()'''
		while True:
			tree = libnw.parse_tree()
			if bool(tree):
				yield Tree(tree.contents)
			else:
				return

	def __init__(self, tree):
		self.tree = tree
		self.root = tree.root.contents

	def to_newick(self):
		return libnw.to_newick(self.tree.root)

	def get_nodes(self):
		'''Returns an iterator over all the tree's nodes, in post-order'''
		nodes_in_order = Llist(self.tree.nodes_in_order.contents)
		for data in nodes_in_order:
			node_p = cast(data, POINTER(rnode))
			yield Rnode(node_p.contents)

	def get_depth(self):
		'''Returns the tree's depth, i.e. the depth of the deepest leaf (in branch length units).'''
		nodelist = list(self.get_nodes())
		#nodelist.reverse()
		for node in nodelist:
			print "%s: %d" % (node.get_label(), node.get_edge_length())

	def get_ancestor_depth(self):
		'''Returns the tree's ancestor depth, i.e. the number of ancestors of the deepest leaf
		(in numbers of ancestors)'''
		pass
