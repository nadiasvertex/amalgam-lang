/*
 * module.h
 *
 *  Created on: May 8, 2012
 *      Author: Christopher Nelson
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "method.h"

namespace amalgam {
namespace parser {

class module;
/** The type for shared pointers to methods. */
typedef std::shared_ptr<module> module_ptr_t;

/** The type for lists of methods. */
typedef std::vector<module_ptr_t> module_list_t;

class module {
   /** The name of the module. */
   std::string name;

   /** The path to the module. */
   std::string path;

   /** The list of methods internal to this module. */
   method_map_t methods;

   /** Methods may be nested. In fact, they are always nested
    * inside at least one method, named '__default__', which
    * contains all methods and variables at module scope. */
   method_list_t nested_method_stack;

   /** The current method (this is the focus of the parsing
    * effort at any time. There is always a current method. */
   method_ptr_t current_method;

public:
   module() {
      add_method(method_ptr_t(new method("__default__")));
   }

   /** This should be used when entering a new method in the
    * parse stream. Pushes the old current method onto the
    * nested method stack, and then sets the current method
    * to the method with the given name.
    */
   void push_current_method(const std::string &name) {
      nested_method_stack.push_back(current_method);
      current_method = methods[name];
   }

   /** This should be used when exiting a method in the
    * parse stream. It sets the current method to whatever
    * is at the top of the nested method stack, and then pops
    * the stack.
    */
   void pop_current_method() {
      current_method = nested_method_stack.back();
      nested_method_stack.pop_back();
   }

   /** Provides a handle to the current method. This always points
    * to something.
    */
   method_ptr_t get_current_method() {
      return current_method;
   }

   /** Adds a method to the module. */
   void add_method(method_ptr_t m) {
      methods[m->get_name()] = m;
   }

   /** Indicates if the module has the named method. */
   auto has_method(const std::string &name) -> bool {
      return methods.find(name) != methods.end();
   }

   /** Gets a handle to the named method. If the method does not
    * exist, this code will fail. */
   auto get_method(const std::string &name) -> method_ptr_t {
      return methods[name];
   }

};

} // end parser namespace
} // end amalgam namespace

#endif /* MODULE_H_ */
