/*
 * parser.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Christopher Nelson
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "rules.h"

namespace amalgam {
namespace parser {

class parser {

public:
   void parse(const std::string &s) {

      ast_stack_t t;

      pegtl::basic_parse_string< grammar >(s, t);

      std::cout << "parsed " << t.size() << " expression trees." << std::endl;

      for(auto n : t) {
         std::cout << (int)n->type << ": '" << n->data << "'" << std::endl;
      }
   }
};

} // end parser namespace
} // end fusion namespace

#endif /* PARSER_H_ */
