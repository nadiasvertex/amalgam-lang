/*
 * machine.h
 *
 *  Created on: Aug 14, 2012
 *      Author: cnelson
 */

#ifndef MACHINE_H_
#define MACHINE_H_

#include <string>
#include <memory>

#include <llvm/Support/IRBuilder.h>

#include "util/expected.h"

namespace amalgam {
namespace machine {

// Forward-declare templ.
class templ;

typedef std::shared_ptr<llvm::Value> value_t;

/// Identifies an operation.
class op {
protected:
	/// If set to true, this
	bool prepared;
	void set_prepared(bool _prepared) {
		prepared = _prepared;
	}
public:
	typedef std::shared_ptr<op> ptr_t;

	op():prepared{false} {};
	virtual ~op() {}

	/// Prepares this operation for code generation.
	virtual auto prepare(templ& t, llvm::IRBuilder<>& builder) -> expected<bool> = 0;

	/// Returns true when the op is prepared for code gen.
	bool is_prepared() {
		return prepared;
	}

	/// Returns true if the operation introduces a new register.
	virtual bool has_output() {
		return false;
	}

	/// Gets the name of the output register.
	virtual std::string get_target_name() {
		return std::string { };
	}

	/// Gets the value of the output register.
	virtual value_t get_target_value() {
		return value_t(nullptr);
	}
};

/// Implements the code needed for an operation that has output
class output_op: public op {
protected:
	/// Register name for output
	std::string target;

	/// The value of the output
	value_t targetv;

public:
	output_op(std::string _target) :
			target { _target } {
	}

	virtual ~output_op() {}

	/// Returns true
	virtual bool has_output() {
		return true;
	}

	/// Gets the name of the output register.
	virtual std::string get_target_name() {

		return target;
	}

	/// Gets the value of the output register.
	virtual value_t get_target_value() {
		return targetv;
	}
};

} // end parser namespace
} // end amalgam namespace

#endif /* MACHINE_H_ */
