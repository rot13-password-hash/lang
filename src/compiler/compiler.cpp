#include "compiler.h"
#include "parser/parser.h"
#include "code_gen/code_gen.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/SystemUtils.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Option/Option.h>
#include <llvm/Support/Program.h>

#include <fstream>
#include <llvm\IR\Verifier.h>

using namespace seam::compiler;

void compiler::link(const std::vector<llvm::StringRef>& object_files, const llvm::StringRef& entry, const llvm::StringRef& output)
{ // ../lld-link.exe test.o Test.lib /OUT:test.exe /ENTRY:test@@constructor
	// TODO: escape file names so it supports path with spaces
	std::filesystem::path p = argv0;
	p.replace_filename("lld-link.exe");
	std::string lld_executable{ p.string() };

	std::vector<llvm::StringRef> link_args;
	link_args.push_back(lld_executable);
	link_args.insert(link_args.end(), object_files.cbegin(), object_files.cend());

	auto out_arg = ("/OUT:" + output).str();
	link_args.push_back(out_arg);

	link_args.push_back("/SUBSYSTEM:CONSOLE");

	auto entry_arg = ("/ENTRY:" + entry).str();
	link_args.push_back(entry_arg);

	auto status = llvm::sys::ExecuteAndWait(lld_executable, llvm::makeArrayRef(link_args), llvm::None);
	if (status == -1)
	{
		throw std::runtime_error("linker failed to execute, make sure you have lld-link (included as part of clang) in your path");
	}
	else if (status == -2)
	{
		throw std::runtime_error("crash during linking process");
	}
}

void seam::compiler::compiler::compile_bitcode(const llvm::StringRef& bc_file, const llvm::StringRef& output)
{
	// TODO: escape file names so it supports path with spaces
	std::filesystem::path p = argv0;
	p.replace_filename("llc.exe");
	std::string llc_executable{ p.string() };

	std::vector<llvm::StringRef> llc_args;
	llc_args.push_back(llc_executable);

	llc_args.push_back("-o");
	llc_args.push_back(output);

	llc_args.push_back("-filetype=obj");

	llc_args.push_back(bc_file);

	auto status = llvm::sys::ExecuteAndWait(llc_executable, llvm::makeArrayRef(llc_args), llvm::None);
	if (status == -1)
	{
		throw std::runtime_error("bitcode compiler failed to execute, make sure you have llc in the same folder as the compiler");
	}
	else if (status == -2)
	{
		throw std::runtime_error("crash during bitcode compiling process");
	}
}

compiler::compiler(const char* argv0, compiler_options opt) :
	argv0(argv0), opt(std::move(opt))
{
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmParsers();
	llvm::InitializeAllAsmPrinters();
}

llvm::Error compiler::compile()
{
	std::error_code error_code;

	if (std::filesystem::exists(opt.output_directory_path) && !std::filesystem::is_directory(opt.output_directory_path))
	{
		throw std::runtime_error("output path is a file");
	}

	std::filesystem::create_directories(opt.output_directory_path);

	std::string input_filename = opt.input_file_path.stem().string();
	if (!opt.input_file_path.has_extension() || opt.input_file_path.extension() != ".sm")
	{
		throw std::runtime_error("file '" + opt.input_file_path.string() + "' has the incorrect extension");
	}
	
	std::ifstream root_module_file{ opt.input_file_path, std::ios::binary | std::ios::in };
	std::string root_module_source{ std::istreambuf_iterator{ root_module_file }, {} };

	parser::parser parser{ input_filename, root_module_source };
	auto root_module_block = parser.parse();
	if (!root_module_block)
	{
		return root_module_block.takeError();
	}

	ir::ast::module root_module{ input_filename, std::move(*root_module_block), true };

	llvm::Triple target_triple{ llvm::sys::getDefaultTargetTriple() };
	
	code_gen::code_gen gen{ types, context, root_module, target_triple.getTriple() };
	auto llvm_root_module = gen.gen_code();
	
	std::string constructor = root_module.relative_path + "@@constructor";
	if (!llvm_root_module->getFunction(constructor))
	{
		throw std::runtime_error("main module must have constructor");
	}

	auto llvm_root_module_bitcode_path = (opt.output_directory_path / (root_module.relative_path + ".bc")).string();
	llvm::raw_fd_ostream llvm_root_module_bitcode{ llvm_root_module_bitcode_path, error_code };
	if (error_code)
	{
		throw std::system_error(error_code);
	}

	llvm::verifyModule(*llvm_root_module);
	llvm_root_module->print(llvm::outs(), nullptr);
	llvm::outs() << '\n';
	
	llvm::WriteBitcodeToFile(*llvm_root_module, llvm_root_module_bitcode);
	llvm_root_module_bitcode.close();

	auto llvm_root_module_object_path = (opt.output_directory_path / (root_module.relative_path + ".o")).string();
	compile_bitcode(llvm_root_module_bitcode_path, llvm_root_module_object_path);

	if (!opt.no_link)
	{
		auto llvm_root_module_executable_path = (opt.output_directory_path / (root_module.relative_path + ".exe")).string();
		auto runtime_lib = std::filesystem::absolute(opt.output_directory_path / "seam-runtime.lib").string();
		llvm::outs() << runtime_lib << '\n';
		link({ llvm_root_module_object_path, runtime_lib }, constructor, llvm_root_module_executable_path);
	}

	return llvm::Error::success();
}
