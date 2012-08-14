/*
 * operation_implementation.h
 *
 *  Created on: Aug 14, 2012
 *      Author: cnelson
 */

#ifndef OPERATION_IMPLEMENTATION_H_
#define OPERATION_IMPLEMENTATION_H_

#include <llvm/LLVMContext.h>
#include <llvm/Support/IRBuilder.h>

#include "operation.h"
#include "template.h"

namespace amalgam {
namespace machine {

/**
 * Holds a constant value like "1" or "10".
 */
template<typename T>
class constant_value: public output_op {
	T value;
public:
	constant_value(std::string _target, T _value) :
			output_op { _target }, value { _value } {
	}
	virtual ~constant_value() {
	}

	/**
	 * Generates an LLVM value from the constant value stored
	 * in this object.
	 */
	virtual auto prepare(templ& t, llvm::IRBuilder<>& builder) -> expected<bool> {
		auto& ctx = llvm::getGlobalContext();
		targetv = value_t(
				llvm::ConstantInt::get(
						ctx,
		                llvm::APInt(64, value, true)
		        )
		);

		set_prepared(true);

		return true;
	}
};

/**
 * Loads a value from memory. The load is optionally atomic.
 */
class load: public output_op {
	std::string source;

public:
	load(std::string _target, std::string _source) :
			output_op { _target }, source { _source } {
	}

	virtual ~load() {
	}

	virtual auto prepare(templ& t, llvm::IRBuilder<>& builder) -> expected<bool> {
		auto& ctx = llvm::getGlobalContext();
		auto source_op = t.find_operation_by_output(source);

		if (!source_op.valid()) {
			return false;
		}

		auto source_value = source_op.get().get_target_value();
		targetv = builder.CreateLoad(source_value, target);

		return true;
	}
};

} // end machine namespace
} // end amalgam namespace

#endif /* OPERATION_IMPLEMENTATION_H_ */
