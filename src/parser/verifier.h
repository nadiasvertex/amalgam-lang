/*
 * verifier.h
 *
 *  Created on: May 9, 2012
 *      Author: Christopher Nelson
 */

#ifndef VERIFIER_H_
#define VERIFIER_H_

#include "module.h"

namespace amalgam {
namespace parser {

class verifier {

   void
   method(method_ptr_t m) {

   }

public:

   void
   verify(module_ptr_t m) {

      for (auto me : m->get_method_map()) {
         method(me.second);
      }

   }
};

} // end parser namespace
} // end amalgam namespace

#endif /* VERIFIER_H_ */
