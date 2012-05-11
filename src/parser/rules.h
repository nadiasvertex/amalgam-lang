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

/** Matches a literal_integer, and if successful, pushes it on the expression stack. Also
 * provides for space padding. */
struct push_integer : pad<
      ifapply<literal_integer, push_node<node_type::literal_int> >, space> {
};

struct identifier : seq< plus< sor<alpha, one<'_'> > >, star< sor<alnum, one<'_'> > >  > {
};

/** Matches an identifier, and if successful, pushes it on the expression stack. Also
 * provides for space padding. */
struct push_identifier : pad<
      ifapply<identifier, push_node<node_type::identifier> >, space> {
};


struct expr;

struct push_group : pad<
    seq<one<'('>, expr, one<')'> >, space> {};

/** An expression atom is one atomic unit of expression. This could be a single
 * literal, or a parenthetical expression. */
struct expr_atom : sor<push_integer, push_identifier, push_group > {
};

struct literal_op : plus< one<'+', '-', '*', '/', '&', '|', '^', '=', '<', '>', 
                         ':', ';', '[', ']', '{', '}', ',', '.', '?', '\\', 
                         '~', '!', '@', '#', '$', '%' > > {
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

