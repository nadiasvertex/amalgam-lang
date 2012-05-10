/*
 * parser.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Christopher Nelson
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "rules.h"
#include "verifier.h"

namespace amalgam {
namespace parser {

class parser {

public:

   module_ptr_t
   parse(const std::string& s, bool verbose = false) {
      auto m = module_ptr_t(new module("__main__"));
      ast_stack_t t;

      pegtl::basic_parse_string<grammar>(s, t, m);

      if (verbose) {
         m->dump();
      }

      verifier v;

      v.verify(m);

      return m;
   }
};

} // end parser namespace
} // end fusion namespace

#endif /* PARSER_H_ */
