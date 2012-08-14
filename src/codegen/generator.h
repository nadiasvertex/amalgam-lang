/*
 * generator.h
 *
 *  Created on: May 8, 2012
 *      Author: cnelson
 */

#ifndef GENERATOR_H_
#define GENERATOR_H_

#include <map>
#include <functional>

#include <llvm/DerivedTypes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetData.h>

#include "../parser/module.h"

namespace amalgam {
namespace codegen {

class generator {

   typedef std::function<llvm::Value * (llvm::Value *, llvm::Value *)> bin_op_gen_t;

   typedef std::map<std::string, bin_op_gen_t> op_map_t;

   llvm::LLVMContext &ctx;
   llvm::Module *cm;

   //llvm::FunctionPassManager *fpm;
   llvm::ExecutionEngine *ee;
   llvm::IRBuilder<> builder;

   llvm::Value *zero_constant;

   /** Contains a mapping of built-in operators to opcode generators. */
   op_map_t op_map;

   /** This points to the module currently being processed. */
   parser::module_ptr_t current_module;

   /** This points to the method currently being processed. */
   parser::method_ptr_t current_method;

   auto
   constant_int(parser::ast_ptr_t i) -> llvm::Value * {
      return llvm::ConstantInt::get(ctx,
                                    llvm::APInt(64, std::stoi(i->data), true));
   }

   auto
   bin_op(parser::ast_ptr_t op) -> llvm::Value * {
      auto left = get_value(op->children[0]);
      auto right = get_value(op->children[1]);

      if (left==nullptr || right==nullptr) {
         std::cout << "internal error: binary operation expected two operands, but did not find them." << std::endl;

         return nullptr;
      }

      auto it = op_map.find(op->data);
      if (it==op_map.end()) {
         std::cout << "internal error: no generator found for '" << op->data << "'." << std::endl;

         return nullptr;
      }

      // Get the operation generator lambda.
      auto op_gen = it->second;

      // Call the operation generator.
      return op_gen(left,right);
   }

   auto
   get_value(parser::ast_ptr_t n) -> llvm::Value * {
      switch (n->type) {
         default:
            std::cout << "internal error: no processor found for node type '" << (int)n->type << "':'" << n->data << "'" << std::endl;
            return nullptr;

         case parser::node_type::literal_int:
            return constant_int(n);
         case parser::node_type::op:
            return bin_op(n);
         case parser::node_type::group:
            return get_value(n->children[0]);
      }
   }

   void
   method(parser::method_ptr_t m) {
      current_method = m;

      auto method_type = llvm::FunctionType::get(llvm::Type::getInt64Ty(ctx),
                                                 std::vector<llvm::Type *>(),
                                                 false);

      auto method_code = llvm::Function::Create(method_type,
                                                llvm::Function::ExternalLinkage,
                                                m->get_name(),
                                                cm);

      auto method_entry_bb = llvm::BasicBlock::Create(ctx,
                                                      "entry",
                                                      method_code);

      builder.SetInsertPoint(method_entry_bb);

      for (auto expr : m->get_expression_tree_list()) {
         auto value = get_value(expr); // We discard the value because these are
                                       // statements. Eventually we will generate code
                                       // for functional-like methods.

         builder.CreateRet(value);
      }

      //builder.CreateRet(zero_constant);

      // Make sure the code is okay.
      llvm::verifyFunction(*method_code);

      // Run the function pass manager over the code.
      //fpm->run(*method_code);
   }

public:
   generator() :
            ctx(llvm::getGlobalContext()), builder(ctx) {

      llvm::InitializeNativeTarget();
      zero_constant = llvm::ConstantInt::get(ctx, llvm::APInt(64, 0, true));

      // ===------------- Initialize the binary operation generator -----------------=== //

#define BINOPGEN(op) [this](llvm::Value *left, llvm::Value *right) { return builder.Create##op (left,right, #op"tmp"); }

      op_map = {
         // Simple integer
         { "+",  BINOPGEN(Add)  },
         { "-",  BINOPGEN(Sub)  },
         { "*",  BINOPGEN(Mul)  },
         { "/",  BINOPGEN(SDiv) },
         { "%",  BINOPGEN(SRem) },
         { "&",  BINOPGEN(And)  },
         { "|",  BINOPGEN(Or)   },
         { "^",  BINOPGEN(Xor)  },
         { "<<", BINOPGEN(Shl)  },
         { ">>", BINOPGEN(LShr) },

         // Comparison
         { ">=", BINOPGEN(ICmpSGE) },
         { "<=", BINOPGEN(ICmpSLE) },
         { "==", BINOPGEN(ICmpEQ)  },
         { "!=", BINOPGEN(ICmpNE)  },
         { "<",  BINOPGEN(ICmpSLT) },
         { ">",  BINOPGEN(ICmpSGT) }
      };

#undef BINOPGEN
   }

   void
   generate(parser::module_ptr_t m, bool verbose=false) {
      cm = new llvm::Module(m->get_name(), ctx);

      current_module = m;

      for (auto me : m->get_method_map()) {
         method(me.second);
      }

      if (verbose) {
         cm->dump();
      }
   }

   void
   run() {
      std::string error_str;
      ee = llvm::EngineBuilder(cm).setErrorStr(&error_str).create();

      if (!ee) {
         std::cout
         << "internal error: unable to create JIT engine: "
         << error_str
         << std::endl;
         return;
      }

      auto entry_point = cm->getFunction("__default__");

      if (!entry_point) {
         std::cout
         << "internal error: unable to locate function '__default__'"
         << std::endl;
         return;
      }

      auto fptr = ee->getPointerToFunction(entry_point);

      if (!fptr) {
         std::cout << "internal error: unable to compile code." << std::endl;
         return;
      }

      auto call = (long long
               (*)())fptr;

      std ::cout << "[ " << call() << std::endl;
   }
}
;
// end generator class

}// end codegen namespace
} // end amalgam namespace

#endif /* GENERATOR_H_ */
