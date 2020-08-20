#include "code_gen.h"

#include "../utils/exception.h"
#include "../ir/ast/ast.h"

using namespace lang::compiler;

struct code_gen_visitor
{

};

code_gen::code_gen::code_gen(llvm::LLVMContext& context, const std::string& filename) : 
	mod(filename, context),
	builder(context)
{}

std::shared_ptr<llvm::Module> code_gen::code_gen::gen_code()
{

}