/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/** \file
 *  \brief An expression node for \c nw_ed
 *  \deprecated \c nw_ed has been replaced by \c nw_sched. */

/** \cond */
struct rnode;

/** The type of node. */

/** Various types of struct enode. \deprecated */

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
	ENODE_NB_DESCENDANTS,
	ENODE_NB_CHILDREN,
	ENODE_SUPPORT,
	ENODE_IS_LEAF,
	ENODE_IS_INNER,
	ENODE_IS_ROOT
};

/** A node for an \c nw_ed expression. \deprecated */

struct enode {
	enum enode_type type;	/**< see enum enode_type **/
	struct enode *left;	/**< left child (or NULL) */
	struct enode *right;	/**< right child (or NULL) */
	float value;		/**< numeric value (not always used) */
};

/** Creates an enode of type ENODE_CONSTANT, with value 'value'.
 * \deprecated */

struct enode *create_enode_constant(float value);

/** Creates an operator enode, of type 'operator', with operands 'left' and
 * 'right'. \deprecated */

struct enode *create_enode_op(enum enode_type type, struct enode *left,
		struct enode *right);

/** Creates a node that negates the argument
 * \deprecated */

struct enode *create_enode_not(struct enode *node);

/** Creates a node for a function 
 * \deprecated */

struct enode *create_enode_func(enum enode_type type);

/** Sets the current tree node, i.e. the one on which the functions will
 * operate. I *could* set 'current_node' directly, but then * it would have to
 * be non-static. I prefer to keep it visible only here. Maybe * I did too much
 * OO.s
 * \deprecated */

void enode_eval_set_current_rnode(struct rnode *);

/** Evaluates an enode. If the enode is an operator, evaluates its operands
 * first.
 * \deprecated */

float eval_enode(struct enode *expr);

/** \endcond */
