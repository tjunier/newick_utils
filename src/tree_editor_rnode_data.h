/* This is a structure that can hold many node properties. It is used e.g. in
 * the tree editor, when the properties that will actually be used are not
 * known in advance: the nodes are first traversed to fill this structure.
 * Contrast this with e.g. displaying: the node properties that we need are
 * their horizontal and vertical positions, and only those */

struct rnode_data {
	float support;	/* normally derived from label */
	float depth;	/* from root; normally root's depth == 0 */
	int nb_ancestors;	/* root has 0 */
	int mark;	/* allows arbitrary marking of nodes */
};
