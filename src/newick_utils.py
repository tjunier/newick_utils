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
libnw.children_count.argtypes = [POINTER(rnode)]

libnw.is_cladogram.argtypes = [POINTER(rooted_tree)]

################################################################
# User-land Python classes

class Llist(object):

	def __init__(self, llist):
		self.llist = llist
		current = self.llist.head
		self.py_list = []
		while bool(current):
			self.py_list.append(current.contents.data)
			current = current.contents.next

	def __iter__(self):
		return iter(self.py_list)

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

	def get_label(self):
		return self.rnode.label

	def get_parent(self):
		parent_addr = cast(self.rnode.parent, c_void_p).value
		return Rnode.c_addr_to_py_obj[parent_addr]

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

	# Some class constants
	PHYLOGRAM = 'Phylogram'
	CLADOGRAM = 'Cladogram'

	@classmethod
	def parse_newick_input(cls):
		'''A generator method that yields trees.
		Usage e.g. 'for tree in parse_newick_input():' '''
		while True:
			tree = libnw.parse_tree()
			if bool(tree):
				yield Tree(tree.contents)
			else:
				return

	def __init__(self, tree):
		self.tree = tree
		self.root = tree.root.contents
		nodes_in_order = Llist(self.tree.nodes_in_order.contents)
		self.depth = None
		# Create an Rnode for each C rnode structure
		for data in nodes_in_order:
			c_node_p = cast(data, POINTER(rnode))
			Rnode(c_node_p)

	def to_newick(self):
		'''Returns a Newick representation of the tree.'''
		return libnw.to_newick(self.tree.root)

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
		if bool(libnw.is_cladogram(self.tree)):
			return Tree.CLADOGRAM
		else:
			return Tree.PHYLOGRAM

	def get_leaf_count(self):
		count = 0
		for node in self.get_nodes():
			if node.is_leaf():
				count += 1
		return count
