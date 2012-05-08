/*
 * rules.h
 *
 *  Created on: May 8, 2012
 *      Author: cnelson
 */

#ifndef RULES_H_
#define RULES_H_

#include "actions.h"

namespace amalgam {
namespace parser {

using namespace pegtl;

/** 
 * A literal integer is: + / - (0-9)+ (a-zA-Z)*
 *
 * Postive / negative specifier
 * Number
 * Specifier
 * 
 */
struct literal_integer : seq<opt<one<'+', '-'> >, plus<digit>, opt<plus<alpha> > > {
};

struct push_integer : pad<
      ifapply<literal_integer, push_node<node_type::literal_int> >, space> {
};

struct expr;

struct expr_atom : sor<push_integer, seq<one<'('>, expr, one<')'> > > {
};

struct literal_op : one<'+', '-', '*', '/', '&', '|', '^'> {
};

struct push_op : pad<ifapply<literal_op, push_node<node_type::op> >, space> {
};

struct expr : list<expr_atom, push_op> {
};

struct expr_list : ifapply<until<eol, expr>, sweep_expression_tree> {
};

struct grammar : expr_list {
};

} // end parser namespace
} // end amalgam namespace

#endif /* RULES_H_ */

