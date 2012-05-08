/*
 * parser.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Christopher Nelson
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "rules.h"
#include "module.h"

namespace amalgam {
namespace parser {

class parser {

public:
   void print_children(ast_ptr_t n, int indent) {
      auto indent_str = std::string(indent, ' ');
      for (auto c : n->children) {
         std::cout
            << indent_str
            << "|"
            << (int) (c->type)
            << ": '"
            << c->data
            << "'"
            << std::endl;

         print_children(c, indent+1);
      }
   }

   void
   parse(const std::string& s) {
      auto m = module_ptr_t(new module());
      ast_stack_t t;

      pegtl::basic_parse_string < grammar > (s, t, m);

      std::cout << "parsed " << t.size() << " expression trees." << std::endl;

      for (auto n : t) {
         std::cout << (int) (n->type) << ": '" << n->data << "'" << std::endl;

         print_children(n, 1);
      }



   }
};

} // end parser namespace
} // end fusion namespace

#endif /* PARSER_H_ */
