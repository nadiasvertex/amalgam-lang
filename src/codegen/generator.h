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
   llvm::LLVMContext &ctx;
   llvm::Module *cm;
   llvm::IRBuilder<> builder;

   llvm::Value *zero_constant;

   auto
   constant_int(parser::ast_ptr_t i) -> llvm::Value * {
      return llvm::ConstantInt::get(ctx,
            llvm::APInt(64, std::stoi(i->data), true));
   }

   auto
   bin_op(parser::ast_ptr_t op) -> llvm::Value * {
      auto left = get_value(op->children[0]);
      auto right = get_value(op->children[1]);

      switch (op->data[0]) {
      case '+':
         return builder.CreateAdd(left, right, "addtmp");
      case '-':
         return builder.CreateSub(left, right, "subtmp");
      case '*':
         return builder.CreateMul(left, right, "multmp");
      case '/':
         return builder.CreateSDiv(left, right, "divtmp");
      }
   }

   auto
   get_value(parser::ast_ptr_t n) -> llvm::Value * {
      switch (n->type) {
      case parser::node_type::literal_int:
         return constant_int(n);
      case parser::node_type::op:
         return bin_op(n);
      }
   }

   void
   method(parser::method_ptr_t m) {
      auto method_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(ctx),
            std::vector<llvm::Type *>(), false);

      auto method_code = llvm::Function::Create(method_type,
            llvm::Function::ExternalLinkage, m->get_name(), cm);

      auto method_entry_bb = llvm::BasicBlock::Create(ctx, "entry",
            method_code);

      builder.SetInsertPoint(method_entry_bb);

      for (auto expr : m->get_expression_tree_list()) {
         auto value = get_value(expr); // We discard the value because these are
                                       // statements. Eventually we will generate code
                                       // for functional-like methods.
      }

      builder.CreateRet(zero_constant);
   }

public:
   generator() :
         ctx(llvm::getGlobalContext()), builder(ctx) {

      zero_constant = llvm::ConstantInt::get(ctx, llvm::APInt(8, 0, true));
   }

   void
   generate(parser::module_ptr_t m) {
      cm = new llvm::Module(m->get_name(), ctx);

      for (auto me : m->get_method_map()) {
         method(me.second);
      }

      cm->dump();
   }
};
// end generator class

}// end codegen namespace
} // end amalgam namespace

#endif /* GENERATOR_H_ */
