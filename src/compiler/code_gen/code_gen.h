#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <string>
#include <memory>

namespace lang::compiler::code_gen
{
	class code_gen
	{
		std::shared_ptr<llvm::Module> mod;
		llvm::IRBuilder<> builder;

	public:
		code_gen(llvm::LLVMContext& context, const std::string& filename);

		std::shared_ptr<llvm::Module> gen_code();
	};
}