/*
 * actions.h
 *
 *  Created on: May 8, 2012
 *      Author: Christopher Nelson
 */

#ifndef ACTIONS_H_
#define ACTIONS_H_

#include <vector>

#include "pegtl.hh"
#include "module.h"

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

/**
 * This action is performed when basic expression elements are recognized. It
 * is used to build up a parse tree of the expressions.
 */
template<node_type nt>
   struct push_node : action_base<push_node<nt> > {
      static void
      apply(const std::string &s, ast_stack_t &t, module_ptr_t m) {
         auto n = make_ast();

         n->type = nt;
         n->data = s;

         switch (nt) {
            default:
               t.push_back(n);
               break;

            case node_type::op:
               if (t.size() > 0 && t.back()->type == node_type::literal_int) {
                  // If the top of the stack is a literal or identifier, swap places with it.
                  auto top = t.back();

                  t.pop_back();
                  t.push_back(n);
                  t.push_back(top);
               } else {
                  t.push_back(n);
               }
               break;
         } // end switch
      } // end apply
   };

struct sweep_expression_tree : action_base<sweep_expression_tree> {
   static auto
   to_tree(ast_stack_t::iterator& it) -> ast_ptr_t {
      auto node = *it;

      switch (node->type) {
         default:
            break;

         case node_type::op:
            node->children.push_back(to_tree(++it));
            node->children.push_back(to_tree(++it));
            break;
      }

      return node;
   }

   static void
   apply(const std::string& s, ast_stack_t& t, module_ptr_t m) {
      // We are at the end of the expression list specified in the file. We sweep
      // the contents of the top of the stack into the current method's expression
      // lists.
      if (t.size() > 0) {
         std::cout << "trace: parse stack:" << std::endl;
         for (auto n : t) {
            std::cout << (int) (n->type) << ": " << n->data << std::endl;
         }

         // sweep the stack recursively
         auto it = t.begin();
         m->get_current_method()->add_expression_tree(to_tree(it));
      }
   }
};

} // end parser namespace
} // end amalgam namespace

#endif /* ACTIONS_H_ */
