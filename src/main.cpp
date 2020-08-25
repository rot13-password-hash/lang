#include "compiler/compiler.h"
#include "compiler/utils/exception.h"
#include "compiler/utils/error.h"

#include "debug/graphviz.h"

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/WithColor.h>

#include <iostream>
#include <memory>

using namespace seam::compiler;

llvm::cl::OptionCategory compiler_category{ "Compiler Options" };

llvm::cl::opt<bool> no_link{ llvm::cl::cat(compiler_category), "c", llvm::cl::desc("Run all stages except linking"),
	llvm::cl::ValueDisallowed };

llvm::cl::opt<std::string> output_directory{ llvm::cl::cat(compiler_category), "o", llvm::cl::desc("Override output directory"),
	llvm::cl::ValueRequired, llvm::cl::init("./out") };

llvm::cl::opt<std::string> input_filename{ llvm::cl::cat(compiler_category), llvm::cl::Positional, llvm::cl::desc("<input file>"),
	llvm::cl::Required };

int main(int argc, char* argv[])
{
	//try
	{
		llvm::ExitOnError exitOnErr{};

		llvm::cl::HideUnrelatedOptions(compiler_category);
		llvm::cl::ParseCommandLineOptions(argc, argv,
			"Seam LLVM compiler");

		compiler_options opt;
		opt.no_link = no_link.getValue();
		opt.output_directory_path = output_directory.getValue();
		opt.input_file_path = input_filename.getValue();

		compiler c{ argv[0], std::move(opt) };
		exitOnErr(c.compile());
		
		return 0;
	}
	/*catch (const exception& ex)
	{
		llvm::errs() << ex.pos.line << ':' << ex.pos.col << ": ";
		llvm::WithColor::error() << ex.what() << '\n';
	}*/
	/*catch (const std::exception& ex)
	{
		llvm::WithColor::error();
		llvm::errs() << ex.what() << '\n';
	}*/
}