#include "compiler/lexer/lexer.h"
#include "compiler/parser/parser.h"
#include "compiler/utils/exception.h"
#include "compiler/ir/cfg/cfg_builder.h"

#include "debug/graphviz.h"

#include <iostream>
#include <memory>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/CodeGen/CommandFlags.inc>
#include <llvm/Transforms/Scalar/GVN.h>

using namespace lang::compiler;

int main() {
    std::string source = R"s(
fn test() -> bool
{
	return true
}

fn main() -> bool @constructor
{
	return test()
}
)s";

	try
	{
		parser::parser parser{ source };
		auto root = parser.parse();

		/*
		graphvizitor gviz;
		root->visit(&gviz);

		std::cout << gviz.string();*/

		// good name?
		cfg_builder::cfg_builder cfg_builder{ root.get() };
		auto cfg = cfg_builder.build();

		auto context = std::make_unique<llvm::LLVMContext>();
		auto builder = std::make_unique<llvm::IRBuilder<>>(*context);
		auto mod = std::make_unique<llvm::Module>("main", *context);

		for (auto& [func, block] : cfg)
		{
			auto function = mod->getFunction(func->name);
			if (function)
			{
				throw std::runtime_error("function redefinition");
			}

			const auto func_type = llvm::FunctionType::get(llvm::Type::getIntNTy(*context, sizeof(int)), false);
			function = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, func->name, *mod);

			llvm::BasicBlock *basic_block = llvm::BasicBlock::Create(*context, "entry", function);
			builder->SetInsertPoint(basic_block);

			for (auto stat : block->body)
			{
				
				stat->gen_code(mod.get(), *builder);
			}

			llvm::verifyFunction(*function);
		}

		mod->print(llvm::errs(), nullptr);

		llvm::InitializeAllTargetInfos();
		llvm::InitializeAllTargets();
		llvm::InitializeAllTargetMCs();
		llvm::InitializeAllAsmParsers();
		llvm::InitializeAllAsmPrinters();

		auto TargetTriple = llvm::Triple(llvm::sys::getDefaultTargetTriple()).get64BitArchVariant();
		mod->setTargetTriple(TargetTriple.getTriple());

		std::string error;
		const auto target = llvm::TargetRegistry::lookupTarget(TargetTriple.getTriple(), error);

		if (!target)
		{
			llvm::errs() << error;
			return 1;
		}

		llvm::TargetOptions opt;
		auto TheTargetMachine = target->createTargetMachine(TargetTriple.getTriple(), "generic", "", opt, llvm::Optional<llvm::Reloc::Model>());

		mod->setDataLayout(TheTargetMachine->createDataLayout());

		std::error_code ec;
		llvm::raw_fd_ostream dest("output.o", ec, llvm::sys::fs::OF_None);

		if (ec)
		{
			llvm::errs() << "Could not open file: " << ec.message();
			return 1;
		}

		llvm::legacy::PassManager pass;
		if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CGFT_ObjectFile, false))
		{
			llvm::errs() << "TheTargetMachine can't emit a file of this type";
			return 1;
		}

		pass.run(*mod);
		dest.flush();

		return 0;
	}
	catch (const exception& ex)
	{
		std::cout << ex.pos.line << ':' << ex.pos.col << ": " << ex.what() << std::endl;
	}
}