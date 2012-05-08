/*
 * generator.h
 *
 *  Created on: May 8, 2012
 *      Author: cnelson
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include "llvm/DerivedTypes.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/IRBuilder.h"

#include "../parser/module.h"

namespace amalgam {
namespace codegen {

class generator {

public:
   void generate(parser::module_ptr_t m) {

   }
};

} // end codegen namespace
} // end amalgam namespace



#endif /* GENERATOR_H_ */
