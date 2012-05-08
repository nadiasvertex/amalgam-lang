/*
 * ast.h
 *
 *  Created on: Apr 10, 2012
 *      Author: Christopher Nelson
 */

#ifndef AST_H_
#define AST_H_

#include <memory>

#include "types.h"

namespace amalgam {
namespace parser {

enum class node_type {
    literal_int,
    identifier,
    op
};

struct ast;
typedef std::shared_ptr<ast> ast_ptr_t;

/** Stores token information. */
struct ast {
    /** The node type. */
    node_type type;
    
    /** Where the node starts in the input stream. */    
    uint64_t start_pos;
    
    /** Where the node ends in the input stream. */    
    uint64_t end_pos;
                
    /** Associated string data (if any) */
    string data; 
    
    /** If there is any alternate data that can
     * be parsed starting at this same position, then
     * we point to it here. */
    ast_ptr_t alt;
};

ast_ptr_t make_ast() {
   return ast_ptr_t(new ast());
}

} // end parser namespace
} // end fusion namespace



#endif /* AST_H_ */
