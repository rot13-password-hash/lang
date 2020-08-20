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
fn main() -> bool @constructor
{
	return true
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

		auto* context = new llvm::LLVMContext();
		auto* builder = new llvm::IRBuilder<>(*context);
		auto mod = std::make_unique<llvm::Module>("main", *context);

		for (auto& [func, block] : cfg)
		{
			llvm::Function *function = mod->getFunction(func->name);
			if (function)
			{
				throw std::runtime_error("function redefinition");
			}

			llvm::FunctionType* func_type = llvm::FunctionType::get(llvm::Type::getIntNTy(*context, sizeof(int)), false);
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

		std::string Error;
		auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple.getTriple(), Error);

		// Print an error and exit if we couldn't find the requested target.
		// This generally occurs if we've forgotten to initialise the
		// TargetRegistry or we have a bogus target triple.
		if (!Target) {
		llvm::errs() << Error;
		return 1;
		}

		auto CPU = "generic";
		auto Features = "";

		llvm::TargetOptions opt;
		auto RM = llvm::Optional<llvm::Reloc::Model>();
		auto TheTargetMachine =
			Target->createTargetMachine(TargetTriple.getTriple(), CPU, Features, opt, RM);

		mod->setDataLayout(TheTargetMachine->createDataLayout());

		auto Filename = "output.o";
		std::error_code EC;
		llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

		if (EC) {
		llvm::errs() << "Could not open file: " << EC.message();
		return 1;
		}

		llvm::legacy::PassManager pass;
		
		auto FileType = llvm::CGFT_ObjectFile;
		if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType, false)) {
		llvm::errs() << "TheTargetMachine can't emit a file of this type";
		return 1;
		}

		pass.run(*mod);
		dest.flush();

		llvm::outs() << "Wrote " << Filename << "\n";

		return 0;
	}
	catch (const exception& ex)
	{
		std::cout << ex.pos.line << ':' << ex.pos.col << ": " << ex.what() << std::endl;
	}
}