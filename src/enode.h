struct rnode;

enum enode_type {
	ENODE_CONSTANT,
	ENODE_GT,
	ENODE_GTE,
	ENODE_LT,
	ENODE_LTE,
	ENODE_EQ,
	ENODE_NEQ,
	ENODE_OR,
	ENODE_AND,
	ENODE_NOT,
	ENODE_DEPTH,
	ENODE_NB_ANCESTORS,
	ENODE_SUPPORT,
	ENODE_IS_LEAF,
	ENODE_IS_INNER,
	ENODE_IS_ROOT
};

struct enode {
	enum enode_type type;
	struct enode *left;
	struct enode *right;
	float value;
};

/* Creates an enode of type ENODE_CONSTANT, with value 'value'. */

struct enode *create_enode_constant(float value);

/* Creates an operator enode, of type 'operator', with operands 'left' and
 * 'right'. */

struct enode *create_enode_op(int type, struct enode *left,
		struct enode *right);

/* Creates a node that negates the argument */

struct enode *create_enode_not(struct enode *node);

/* Creates a node for a function */

struct enode *create_enode_func(int type);

/* Sets the current tree node, i.e. the one on which the functions will
 * operate. I *could* set 'current_node' directly, but then * it would have to
 * be non-static. I prefer to keep it visible only here. Maybe * I did too much
 * OO. */

void enode_eval_set_current_rnode(struct rnode *);

/* Evaluates an enode. If the enode is an operator, evaluates its operands
 * first.*/

float eval_enode(struct enode *expr);
