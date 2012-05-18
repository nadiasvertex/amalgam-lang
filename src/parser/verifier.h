/*
 * verifier.h
 *
 *  Created on: May 9, 2012
 *      Author: Christopher Nelson
 */

#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "module.h"
#include "../util/strutil.h"

namespace amalgam {
namespace parser {

class verifier {

   module_ptr_t module;
   bool verbose;

   /** Determines if the expression is an lvalue. If it is, this method
    * will return true. Otherwise it will return false.
    */
   bool
   is_lvalue(ast_ptr_t e) {
      if (e->type == node_type::identifier) {
         return true;
      }
      return false;
   }

   /** Find the most significant set bit. */
   template<typename T>
      auto
      get_most_significant_bit(T value) -> int {
         auto r = 0;

         // Find the most significant bit set. This will tell us how big the integer has to be,
         // in bits.
         while (value >>= 1) {
            ++r;
         }

         return r;
      }

   type_annotation::ptr_t
   get_int_type(const string& token) {
      auto base = 10;
      auto is_signed = true;

      auto num_spec = split_literal_int(token);

      if (num_spec.second.size()) {
         if (num_spec.second == "h") {
            base = 16;
         } else if (num_spec.second == "b") {
            base = 2;
         } else if (num_spec.second == "o") {
            base = 8;
         } else if (num_spec.second == "Uh") {
            base = 16;
            is_signed = false;
         } else if (num_spec.second == "Ub") {
            base = 2;
            is_signed = false;
         } else if (num_spec.second == "Uo") {
            base = 8;
            is_signed = false;
         } else {
            // Unknown specifier. This needs to be handled by
            // checking the module for literal handlers.
         }
      }

      // Figure out how big the integer has to be.
      int msb;
      if (is_signed) {
         msb = get_most_significant_bit(std::stoll(num_spec.first,
                                                   nullptr,
                                                   base));
      } else {
         msb = get_most_significant_bit(std::stoull(num_spec.first,
                                                    nullptr,
                                                    base));
      }

      auto t = new numeric_type_annotation(type_annotation::type_id::integer);

      t->is_signed = is_signed;
      t->specifier = num_spec.second;

      if (msb <= 8) {
         t->size_in_bits = 8;
      } else if (msb <= 16) {
         t->size_in_bits = 16;
      } else if (msb <= 32) {
         t->size_in_bits = 32;
      } else if (msb <= 64) {
         t->size_in_bits = 64;
      }

      return type_annotation::ptr_t(t);
   }

   /** Check the tree and get the type of the node by evaluating the type information. */
   type_annotation::ptr_t
   get_type(ast_ptr_t e) {
      if (e->semantic_type) {
         return e->semantic_type;
      }

      switch (e->type) {
         case node_type::op: {
            auto l_type = get_type(e->children[0]);
            auto r_type = get_type(e->children[1]);

            if (!l_type || !r_type || l_type != r_type) {
               break;
            }
         }
            break;

         case node_type::literal_int: {
            e->semantic_type = get_int_type(e->data);
         }
            break;
      }

      return e->semantic_type;
   }

   /** Evaluate the tree and provide type annotations for each element in the tree. Also check
    * to make sure that the expression is semantically valid. This may also perform some
    * book-keeping for the method regarding variable presence and initialization.
    */
   bool
   expression_tree(method_ptr_t m, ast_ptr_t e) {
      // If we have an initialization operator, the left side
      // must be an identifier.
      if (e->type == node_type::op) {
         if (e->data == ":=") {
            if (e->children.size() == 0 || (!is_lvalue(e->children[0]))) {
               return false;
            }

            // Get the type of the rvalue
            auto r_type = get_type(e->children[1]);
            if (!r_type) {
               std::cout << "error: unable to infer type for the right hand side of the initializer" << std::endl;
               return false;
            }

            e->children[0]->semantic_type = r_type;
            m->add_variable(e->children[0]->data, r_type);
         } else {
            auto l_type = get_type(e->children[0]);
            auto r_type = get_type(e->children[1]);



         }
      }

      return true;
   }

   bool
   method(method_ptr_t m) {
      auto passed = true;
      for (auto e : m->get_expression_tree_list()) {
         if (!expression_tree(m, e)) {
            passed = false;
         }
      }

      return passed;
   }

public:

   bool
   verify(module_ptr_t m, bool _verbose = false) {
      module = m;
      verbose = _verbose;

      auto passed = true;
      for (auto me : m->get_method_map()) {
         if (!method(me.second)) {
            passed = false;
         }
      }

      return passed;
   }
};

} // end parser namespace
} // end amalgam namespace

#endif /* VERIFIER_H_ */
