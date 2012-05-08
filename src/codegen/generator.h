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
   llvm::Module *cm;
   llvm::IRBuilder<> builder;

   auto constant_int(parser::ast_ptr_t i) -> llvm::Value * {
      return llvm::ConstantInt::get(llvm::getGlobalContext(), llvm::APInt(64, std::stoi(i->data), true));
   }

   auto bin_op(parser::ast_ptr_t op) -> llvm::Value * {
      auto left = get_value(op->children[0]);
      auto right = get_value(op->children[1]);

      switch(op->data[0]) {
      case '+': return builder.CreateAdd(left, right, "addtmp");
      case '-': return builder.CreateSub(left, right, "subtmp");
      case '*': return builder.CreateMul(left, right, "multmp");
      case '/': return builder.CreateSDiv(left, right, "divtmp");
      }
   }

   auto get_value(parser::ast_ptr_t n) -> llvm::Value * {
      switch(n->type) {
      case parser::node_type::literal_int:
         return constant_int(n);
      case parser::node_type::op:
         return bin_op(n);
      }
    }

   void method(parser::method_ptr_t m) {

   }

public:
   generator():builder(llvm::getGlobalContext()) {}

   void generate(parser::module_ptr_t m) {
      for(auto me : m->get_method_map()) {
         method(me.second);
      }
   }
}; // end generator class

} // end codegen namespace
} // end amalgam namespace



#endif /* GENERATOR_H_ */
