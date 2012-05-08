/*
 * method.h
 *
 *  Created on: May 8, 2012
 *      Author: Christopher Nelson
 */

#ifndef METHOD_H_
#define METHOD_H_

#include <map>

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
   std::string name;

   /** The list of expressions to evaluate in this method. */
   ast_list_t expression_list;

public:
   method(const std::string _name):name(_name) {}

   /** Gets the name of the method */
   auto get_name() -> const std::string & { return name; }

   /** Adds a completely processed expression tree to the list
    * of expressions this method must process. */
   void add_expression_tree(ast_ptr_t node) {
      expression_list.push_back(node);
   }
};


} // end parser namespace
} // end amalgam namespace

#endif /* METHOD_H_ */
