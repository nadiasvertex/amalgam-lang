/*
 * template.h
 *
 *  Created on: Aug 14, 2012
 *      Author: cnelson
 */

#ifndef TEMPLATE_H_
#define TEMPLATE_H_

#include <exception>
#include <string>
#include <vector>
#include <unordered_map>

#include <llvm/LLVMContext.h>
#include <llvm/Support/IRBuilder.h>

#include "operation.h"
#include "util/expected.h"

namespace amalgam {
namespace machine {

class templ {
	typedef std::vector<op::ptr_t> op_list_t;
	typedef std::unordered_map<std::string, op::ptr_t> reg_op_map_t;

	// The name of the template
	std::string name;

	// The list of operations that compose this template.
	op_list_t   op_list;

	// Maps a register name to the operation which produces it.
	reg_op_map_t reg_map;

public:
	typedef std::shared_ptr<templ> ptr_t;

	templ(std::string _name):name{_name} {}

	/// Adds an operation to this template.
	void add_operation(op::ptr_t opcode) {
		op_list.push_back(opcode);

		if (opcode->has_output()) {
			reg_map[opcode->get_target_name()] = opcode;
		}
	}

	/// Finds an operation by using the name of the output register.
	auto find_operation_by_output(std::string output_reg) -> expected<op::ptr_t> {
		auto it = reg_map.find(output_reg);

		if (it == reg_map.end()) {
			return expected<op::ptr_t>::from_exception(std::out_of_range("unknown output register"));
		}

		return it->second;
	}

	auto prepare(llvm::BasicBlock *bb) {
		llvm::IRBuilder<> builder(bb);

		for(auto o : op_list) {
			o->prepare(*this, builder);
		}
	}

};

class template_manager {
	typedef std::unordered_map<std::string, templ::ptr_t> template_name_map_t;

	template_name_map_t templates;
public:

};

} // end parser namespace
} // end amalgam namespace


#endif /* TEMPLATE_H_ */
