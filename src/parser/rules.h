/*
 * rules.h
 *
 *  Created on: May 8, 2012
 *      Author: cnelson
 */

#ifndef RULES_H_
#define RULES_H_

#include <vector>

#include "pegtl.hh"
#include "ast.h"

namespace amalgam {
namespace parser {

using namespace pegtl;

/**
 * This type represents a stack of ast trees. As we
 * recognize input, nodes will be pushed onto
 * the stack. Some nodes will pop items off of the
 * stack and internalize them, then replace the top
 * of the stack with itself.
 */
typedef std::vector<ast_ptr_t> ast_stack_t;

template< node_type nt >
struct push_node : action_base< push_node< nt > > {
    static void apply( const std::string &s, ast_stack_t &t) {
        auto n = make_ast();
        
        n->type = nt;
        n->data = s;
        
        t.push_back( n );
    }
};


/** 
 * A literal integer is: + / - (0-9)+ (a-zA-Z)*
 *
 * Postive / negative specifier
 * Number
 * Specifier
 * 
 */
struct literal_integer : seq< opt< one< '+', '-' > >, plus< digit >, opt< plus< alpha > > > {};

struct expr;

struct expr_atom : sor< literal_integer, seq< one<'('>, expr, one<')'> > > {}; 

struct expr_op : one< '+', '-', '*', '/', '&', '|', '^'> {};

struct expr : plus < sor< expr_atom, expr_op > > {} ; 

struct grammar : until< eol, expr > {};

} // end parser namespace
} // end amalgam namespace


#endif /* RULES_H_ */
