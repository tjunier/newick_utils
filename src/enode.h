
enum enode_type {
	ENODE_CONSTANT,
	ENODE_GT,
	ENODE_GTE,
	ENODE_LT,
	ENODE_LTE,
	ENODE_FUNC
};

struct enode {
	enum enode_type type;
	struct enode *left;
	struct enode *right;
	float value;
	int function;
};

/* Creates an enode of type ENODE_CONSTANT, with value 'value'. */

struct enode *create_enode_constant(float value);

/* Creates an operator enode, of type 'operator', with operands 'left' and
 * 'right'. */

struct enode *create_enode_op(int type, struct enode *left,
		struct enode *right);

/* Evaluates an enode. If the enode is an operator, evaluates its operands
 * first.*/

float eval_enode(struct enode *expr);
