#include "code_gen.h"

#include "../utils/exception.h"
#include "function_collector.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>

#include <cmath>
#include <array>

static_assert(sizeof(float) == 4, "float size non standard");
static_assert(sizeof(double) == 8, "double size non standard");

using namespace lang::compiler;

namespace lang::compiler::code_gen
{
	struct code_gen_visitor : ir::ast::visitor
	{
		code_gen_visitor(code_gen& gen)
			: gen(gen) {}

		code_gen& gen;

		llvm::Value* val;

		bool visit(ir::ast::node* node)
		{
			throw exception(node->range.start, "code generation for this node is not implemented");
		}

		bool visit(ir::ast::expression::expression* node)
		{
			throw exception(node->range.start, "code generation for this expression is not implemented");
		}

		bool visit(ir::ast::statement::statement* node)
		{
			throw exception(node->range.start, "code generation for this statement is not implemented");
		}

		bool visit(ir::ast::statement::restricted_statement* node)
		{
			throw exception(node->range.start, "unexpected restricted statement");
		}

		bool visit(ir::ast::expression::literal<std::string>* node)
		{
			val = gen.builder.CreateGlobalString(llvm::StringRef(node->val));
			return false;
		}

		bool visit(ir::ast::expression::literal<lang::compiler::ir::ast::number>* node)
		{
			throw exception(node->range.start, "unexpected unparsed number");
		}

		bool visit(ir::ast::expression::literal<std::int8_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::int8_t) * 8, static_cast<std::int8_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::int16_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::int16_t) * 8, static_cast<std::int16_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::int32_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::int32_t) * 8, static_cast<std::int32_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::int64_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::int64_t) * 8, static_cast<std::int64_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint8_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::uint8_t) * 8, static_cast<std::uint8_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint16_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::uint16_t) * 8, static_cast<std::uint16_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint32_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::uint32_t) * 8, static_cast<std::uint32_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint64_t>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::uint64_t) * 8, static_cast<std::uint64_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<float>* node)
		{
			val = llvm::ConstantFP::get(gen.mod->getContext(), llvm::APFloat{ node->val });
			return false;
		}

		bool visit(ir::ast::expression::literal<double>* node)
		{
			val = llvm::ConstantFP::get(gen.mod->getContext(), llvm::APFloat{ node->val });
			return false;
		}

		bool visit(ir::ast::expression::literal<bool>* node)
		{
			val = llvm::ConstantInt::get(gen.mod->getContext(), llvm::APInt(sizeof(std::uint8_t) * 8, static_cast<std::uint8_t>(node->val)));
			return false;
		}
	
		// TODO: **disallow** calling of constructors
		bool visit(ir::ast::expression::call* node)
		{
			return true;
		}

		bool visit(ir::ast::expression::unresolved_variable* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::type_definition* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::function_definition* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::alias_type_definition* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::class_type_definition* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::block* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::restricted_block* node)
		{
			return true;
		}

		bool visit(ir::ast::statement::ret* node)
		{
			if (node->value)
			{
				node->value->visit(this);
				gen.builder.CreateRet(val);
			}
			else
			{
				gen.builder.CreateRetVoid();
			}

			return false;
		}
	};
}

llvm::Type* code_gen::code_gen::get_llvm_type(ir::types::type_descriptor* type_desc)
{
	auto it = type_map.find(type_desc);
	if (it != type_map.cend())
	{
		return it->second;
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<void>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getVoidTy(mod->getContext());
	}

	// string, str (immutable)
	// 3, "abc"
	if (dynamic_cast<ir::types::built_in_type_descriptor<std::string>*>(type_desc))
	{
		throw std::runtime_error("not implemented");
		// return type_map[type_desc] = llvm::Type::getStringTy(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<bool>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt8Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int8_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt8Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int16_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt16Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int32_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt32Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int64_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt64Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint8_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt8Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint16_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt16Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint32_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt32Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint64_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt64Ty(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<float>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getFloatTy(mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<double>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getDoubleTy(mod->getContext());
	}
	
	throw std::runtime_error("not implemented");

	return nullptr;
}

llvm::Type* code_gen::code_gen::get_llvm_type(ir::types::type_reference& type_ref)
{
	auto base_type = get_llvm_type(type_ref.type.get());
	if (type_ref.is_optional)
	{
		std::array<llvm::Type*, 2> struct_fields { llvm::Type::getInt8Ty(mod->getContext()), base_type };
		return llvm::StructType::get(mod->getContext(), llvm::makeArrayRef(struct_fields)); // ctx, array, packed (false)
	}

	return base_type;
}

llvm::FunctionType* code_gen::code_gen::get_llvm_function_type(ir::ast::statement::function_definition* func_def)
{
	auto ret_type = get_llvm_type(std::get<ir::types::type_reference>(func_def->return_type));
	if (!llvm::FunctionType::isValidReturnType(ret_type))
	{
		throw exception(func_def->range.start, "invalid return type");
	}

	std::vector<llvm::Type*> parameter_types;
	parameter_types.reserve(func_def->arguments.size());
	for (auto& param : func_def->arguments)
	{
		auto param_type = get_llvm_type(std::get<ir::types::type_reference>(param.type_));
		if (!llvm::FunctionType::isValidArgumentType(param_type))
		{
			throw exception(func_def->range.start, "invalid argument type");
		}
		parameter_types.push_back(param_type);
	}

	return llvm::FunctionType::get(ret_type, llvm::makeArrayRef(parameter_types), false);
}

code_gen::code_gen::code_gen(std::unordered_map<ir::types::type_descriptor*, llvm::Type*>& type_map, llvm::LLVMContext& context, ir::ast::module& root) :
	root(root),
	type_map(type_map),
	mod(std::make_shared<llvm::Module>(root.relative_path, context)),
	builder(context)
{}

std::shared_ptr<llvm::Module> code_gen::code_gen::gen_code()
{
	function_collector collector;
	root.body->visit(&collector);

	code_gen_visitor gen{ *this };

	for (auto func : collector.collected)
	{
		std::string name;
		llvm::GlobalValue::LinkageTypes linkage;

		// TODO: maybe add destructors?
		bool is_constructor = func->attributes.find("constructor") != func->attributes.cend();

		if (func->attributes.find("export") != func->attributes.cend())
		{
			linkage = llvm::GlobalValue::ExternalLinkage;
			name = root.relative_path + '@' + func->name;
		}
		else
		{
			linkage = llvm::GlobalValue::InternalLinkage;
			name = func->name;
		}

		llvm::Function *function = mod->getFunction(name);
		if (!function)
		{
			// TODO: make get function type function
			// llvm::Type::getIntNTy(mod->getContext(), sizeof(bool))
			llvm::FunctionType* func_type = get_llvm_function_type(func);
			function = llvm::Function::Create(func_type, linkage, name, *mod);
			if (is_constructor) // TODO: check that constructor is void()
			{
				if (func_type->getReturnType() != llvm::Type::getVoidTy(mod->getContext())
					|| func_type->getNumParams() != 0)
				{
					throw exception(func->range.start, "constructor must return void and have no parameters");
				}
				llvm::appendToGlobalCtors(*mod, function, 0);
			}
		}

		llvm::BasicBlock *basic_block = llvm::BasicBlock::Create(mod->getContext(), "entry", function);
		builder.SetInsertPoint(basic_block);

		func->visit(&gen);

		if (basic_block->empty() || !llvm::isa<llvm::ReturnInst>(basic_block->back()))
		{
			builder.CreateRetVoid();
		}

		llvm::verifyFunction(*function);
	}

	return mod;
}