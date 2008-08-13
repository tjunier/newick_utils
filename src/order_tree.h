/* order_tree.h: functions for ordering trees */

/* A helper function, to pass as parameter to qsort() */

int lbl_comparator(const void *, const void *);

/* Orders all nodes in the tree, according to lbl_comparator() - this could be
 * parametered, if we can think of other interesting orders. NOTE: this
 * function uses (and modifies) the rnode's data attribute. */

void order_tree(struct rooted_tree *tree);
