#pragma once

#include "ir/ast/types.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>

#include <filesystem>
#include <unordered_map>
#include <string>

namespace seam::compiler
{
	struct compiler_options
	{
		bool no_link;
		std::filesystem::path output_directory_path;
		std::filesystem::path input_file_path;
	};

	class compiler
	{
		compiler_options opt;

		const char* argv0;

		std::unordered_map<ir::types::type_descriptor*, llvm::Type*> types;
		llvm::LLVMContext context;

		void link(const std::vector<llvm::StringRef>& object_files, const llvm::StringRef& entry, const llvm::StringRef& output);
		void compile_bitcode(const llvm::StringRef& bc_file, const llvm::StringRef& output);
	public:
		compiler(const char* argv0, compiler_options opt);

		llvm::Error compile();
	};
}