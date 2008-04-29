struct rooted_tree;

/* Dumps a tree to stdout, as a text graph. 'width' is the width of the output line, in characters. If 'align_leaves' is true, the leaves will be aligned (surprise!) - useful for cladograms.  */

void display_tree(struct rooted_tree *tree, int width, int align_leaves);
