/*
 * method.h
 *
 *  Created on: May 8, 2012
 *      Author: Christopher Nelson
 */

#ifndef METHOD_H_
#define METHOD_H_

#include <map>

#include "annotations.h"
#include "ast.h"

namespace amalgam {
namespace parser {

class method;
/** The type for shared pointers to methods. */
typedef std::shared_ptr<method> method_ptr_t;

/** The type for lists of methods. */
typedef std::vector<method_ptr_t> method_list_t;

/** The type for maps of methods. */
typedef std::map<std::string, method_ptr_t> method_map_t;

class method {
   /** The name of the method. */
   string name;

   /** The list of expressions to evaluate in this method. */
   ast_list_t expression_list;

   /** The type of the method. */
   method_type_annotation type;

   /** The list of variables declared in this method. */
   type_annotation::map_t vars;

public:
   method(const std::string _name) :
         name(_name) {
   }

   /** Gets the name of the method */
   auto
   get_name() -> const std::string & {
      return name;
   }

   //=====----------------------------------------------------------------------======//
   //      Expression Trees
   //=====----------------------------------------------------------------------======//


   /** Adds a completely parsed expression tree to the list
    * of expressions this method must process. */
   void
   add_expression_tree(ast_ptr_t node) {
      expression_list.push_back(node);
   }

   /** Gets the expression tree list for iteration purposes. */
   auto get_expression_tree_list() -> const ast_list_t & {
      return expression_list;
   }

   //=====----------------------------------------------------------------------======//
   //      Variables
   //=====----------------------------------------------------------------------======//


   /** Adds a variable to this method. This is generally called by
    * the verifier during processing of the expression list.*/
   void add_variable(string name, type_annotation::ptr_t t) {
      vars[name] = t;
   }

   /** Looks for a variable declared in this method with the given name. */
   bool has_variable(string name) {
      return vars.find(name) != vars.end();
   }

   //=====----------------------------------------------------------------------======//
   //      Parser Debugging and Instrumentation
   //=====----------------------------------------------------------------------======//

   void
   dump() {
      for (auto n : expression_list) {
         std::cout << std::endl
                   << (int) (n->type) << ":'" << n->data << "'" << std::endl;

         print_children(n, 1);
      }
   }

private:
   void
   print_children(ast_ptr_t n, int indent) {
      auto indent_str = std::string(indent, ' ');
      for (auto c : n->children) {
         std::cout << indent_str << "|" << (int) (c->type) << ": '" << c->data
               << "'" << std::endl;

         print_children(c, indent + 1);
      }
   }

};

} // end parser namespace
} // end amalgam namespace

#endif /* METHOD_H_ */
