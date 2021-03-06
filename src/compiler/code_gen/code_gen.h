#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <string>
#include <memory>
#include "../ir/ast/ast.h"


namespace seam::compiler::code_gen
{
	struct code_gen_visitor;

	class code_gen
	{
		friend code_gen_visitor;

		ir::ast::module& mod;

		std::shared_ptr<llvm::Module> llvm_mod;
		std::unique_ptr<llvm::DataLayout> data_layout;
		llvm::IRBuilder<> builder;

		std::unordered_map<ir::types::type_descriptor*, llvm::Type*>& type_map;

		llvm::Type* get_llvm_type(ir::types::type_descriptor* type_desc);
		llvm::Type* get_llvm_type(ir::types::type_reference& type_ref);

		llvm::FunctionType* get_llvm_function_type(ir::ast::statement::function_declaration* func_def);

		llvm::Function* get_or_declare_function(const std::string& symbol, ir::ast::statement::function_declaration* def_stat);
	public:
		code_gen(std::unordered_map<ir::types::type_descriptor*, llvm::Type*>& type_map, llvm::LLVMContext& context, ir::ast::module& root, const std::string& target_triple);

		std::shared_ptr<llvm::Module> gen_code();
	};
}