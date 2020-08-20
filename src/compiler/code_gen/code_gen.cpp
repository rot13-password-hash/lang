#include "code_gen.h"

#include "../utils/exception.h"
#include "../ir/ast/ast.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

using namespace lang::compiler;
using namespace llvm;

Value* ir::ast::statement::restricted_block::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::expression::call::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}


Value* ir::ast::statement::expression_statement::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::expression::unresolved_variable::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::statement::class_type_definition::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::statement::alias_type_definition::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::statement::function_definition::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::statement::block::gen_code(Module* mod, IRBuilder<>& builder)
{
	throw exception(range.start, "unexpected block while generating code");
}

Value* ir::ast::node::gen_code(Module* mod, IRBuilder<>& builder)
{
	throw exception(range.start, "code generation for this node is not implemented");
}

Value* ir::ast::expression::literal<std::string>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ir::ast::node::gen_code(mod, builder);
}

Value* ir::ast::expression::literal<ir::ast::number>::gen_code(Module* mod, IRBuilder<>& builder)
{
	throw exception(range.start, "unparsed number found while generating code");
}

Value* ir::ast::expression::literal<std::int8_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::int8_t), val));
}

Value* ir::ast::expression::literal<std::int16_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::int16_t), val));
}

Value* ir::ast::expression::literal<std::int32_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::int32_t), val));
}

Value* ir::ast::expression::literal<std::int64_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::int64_t), val));
}

Value* ir::ast::expression::literal<std::uint8_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::uint8_t), val));
}

Value* ir::ast::expression::literal<std::uint16_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::uint16_t), val));
}

Value* ir::ast::expression::literal<std::uint32_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::uint32_t), val));
}

Value* ir::ast::expression::literal<std::uint64_t>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(std::uint64_t), val));
}

Value* ir::ast::expression::literal<float>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantFP::get(mod->getContext(), APFloat(val));
}

Value* ir::ast::expression::literal<double>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantFP::get(mod->getContext(), APFloat(val));
}

Value* ir::ast::expression::literal<bool>::gen_code(Module* mod, IRBuilder<>& builder)
{
	return ConstantInt::get(mod->getContext(), APInt(sizeof(int), static_cast<int>(val)));
}

Value* ir::ast::statement::ret::gen_code(Module* mod, IRBuilder<>& builder)
{
	const auto ret_value = value->gen_code(mod, builder);
	return builder.CreateRet(ret_value);
}