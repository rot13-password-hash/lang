#include "code_gen.h"

#include "../utils/exception.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/ModuleUtils.h>

#include <cmath>
#include <array>
#include <iostream>


#include "../parser/passes/symbol_collector.h"
#include "../parser/passes/variable_resolver.h"

static_assert(sizeof(float) == 4, "float size non standard");
static_assert(sizeof(double) == 8, "double size non standard");

using namespace seam::compiler;

namespace seam::compiler::code_gen
{
	struct code_gen_visitor : ir::ast::visitor
	{
		code_gen_visitor(code_gen& gen)
			: gen(gen) {}

		code_gen& gen;

		llvm::Value* val = nullptr;

		bool visit(ir::ast::node* node) override
		{
			throw exception(node->range.start, "code generation for this node is not implemented");
		}

		bool visit(ir::ast::expression::expression* node) override
		{
			throw exception(node->range.start, "code generation for this expression is not implemented");
		}

		bool visit(ir::ast::statement::statement* node) override
		{
			throw exception(node->range.start, "code generation for this statement is not implemented");
		}

		bool visit(ir::ast::statement::restricted_statement* node) override
		{
			throw exception(node->range.start, "unexpected restricted statement");
		}

		bool visit(ir::ast::expression::literal<std::string>* node) override
		{
			// TODO: test
			// TODO: check if getIntNTy takes bits or bytes
			auto size_type = llvm::Type::getIntNTy(gen.llvm_mod->getContext(), gen.data_layout->getMaxPointerSizeInBits());
			auto char_array_type = llvm::ArrayType::get(llvm::Type::getInt8Ty(gen.llvm_mod->getContext()), node->val.size());

			std::array<llvm::Type*, 2> struct_fields{ size_type, char_array_type };
			auto struct_type = llvm::StructType::get(gen.llvm_mod->getContext(), llvm::makeArrayRef(struct_fields), false);
			auto str_const = llvm::ConstantDataArray::getString(gen.llvm_mod->getContext(), node->val, false);

			auto size_const  = llvm::ConstantInt::get(gen.llvm_mod->getContext(),
				llvm::APInt(gen.data_layout->getMaxPointerSizeInBits(), node->val.size(), false));
			auto struct_const = llvm::ConstantStruct::get(struct_type, size_const, str_const);
			auto global_var = new llvm::GlobalVariable{ *gen.llvm_mod, struct_const->getType(),
				true, llvm::GlobalValue::PrivateLinkage, struct_const };
			global_var->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
			global_var->setAlignment(llvm::Align(1));
			val = global_var;
			auto zero = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(32, 0, true));
			std::array<llvm::Constant*, 2> indicies = {zero, zero};
			val = llvm::ConstantExpr::getInBoundsGetElementPtr(global_var->getValueType(), global_var, llvm::makeArrayRef(indicies)); // size_type*
			/*
			val->getType()->print(llvm::outs());
			llvm::outs() << '\n';*/
			return false;
		}

		bool visit(ir::ast::expression::literal<seam::compiler::ir::ast::number>* node) override
		{
			throw exception(node->range.start, "unexpected unparsed number");
		}

		bool visit(ir::ast::expression::literal<std::int8_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::int8_t) * 8, static_cast<std::int8_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::int16_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::int16_t) * 8, static_cast<std::int16_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::int32_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::int32_t) * 8, static_cast<std::int32_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::int64_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::int64_t) * 8, static_cast<std::int64_t>(node->val), true));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint8_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::uint8_t) * 8, static_cast<std::uint8_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint16_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::uint16_t) * 8, static_cast<std::uint16_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint32_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::uint32_t) * 8, static_cast<std::uint32_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<std::uint64_t>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::uint64_t) * 8, static_cast<std::uint64_t>(node->val)));
			return false;
		}

		bool visit(ir::ast::expression::literal<float>* node) override
		{
			val = llvm::ConstantFP::get(gen.llvm_mod->getContext(), llvm::APFloat{ node->val });
			return false;
		}

		bool visit(ir::ast::expression::literal<double>* node) override
		{
			val = llvm::ConstantFP::get(gen.llvm_mod->getContext(), llvm::APFloat{ node->val });
			return false;
		}

		bool visit(ir::ast::expression::literal<bool>* node) override
		{
			val = llvm::ConstantInt::get(gen.llvm_mod->getContext(), llvm::APInt(sizeof(std::uint8_t) * 8, static_cast<std::uint8_t>(node->val)));
			return false;
		}
	
		// TODO: **disallow** calling of constructors
		bool visit(ir::ast::expression::call* node) override
		{
			node->func->visit(this);
			if (!llvm::isa<llvm::Function>(val))
			{
				throw exception(node->func->range.start, "expected function during code generation");
			}

			auto func_val = reinterpret_cast<llvm::Function*>(val);
			
			std::vector<llvm::Value*> arguments;

			for (auto& arg : node->arguments)
			{
				arg->visit(this);
				arguments.push_back(val);
			}

			val = gen.builder.CreateCall(func_val, llvm::makeArrayRef(arguments));
			return false;
		}

		bool visit(ir::ast::expression::variable* node) override
		{
			return true;
		}

		bool visit(ir::ast::expression::unresolved_variable* node) override
		{
			throw exception(node->range.start, "unresolved variable encountered during code generation");
			return true;
		}

		bool visit(ir::ast::expression::function_variable* node) override
		{
			val = gen.get_or_declare_function(node->symbol, node->def_stat);
			return false;
		}

		bool visit(ir::ast::statement::expression_statement* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::type_definition* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::extern_definition* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::function_definition* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::alias_type_definition* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::class_type_definition* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::block* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::restricted_block* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::ret* node) override
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
		return type_map[type_desc] = llvm::Type::getVoidTy(llvm_mod->getContext());
	}

	// string (immutable)
	// 3, "abc"
	if (dynamic_cast<ir::types::built_in_type_descriptor<std::string>*>(type_desc))
	{
		// TODO: check if getIntNTy takes bits or bytes
		return llvm::PointerType::get(llvm::Type::getIntNTy(llvm_mod->getContext(), data_layout->getMaxPointerSizeInBits()), 0);
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<bool>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt8Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int8_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt8Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int16_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt16Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int32_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt32Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::int64_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt64Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint8_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt8Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint16_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt16Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint32_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt32Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<std::uint64_t>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getInt64Ty(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<float>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getFloatTy(llvm_mod->getContext());
	}

	if (dynamic_cast<ir::types::built_in_type_descriptor<double>*>(type_desc))
	{
		return type_map[type_desc] = llvm::Type::getDoubleTy(llvm_mod->getContext());
	}
	
	throw std::runtime_error("not implemented");

	return nullptr;
}

llvm::Type* code_gen::code_gen::get_llvm_type(ir::types::type_reference& type_ref)
{
	auto base_type = get_llvm_type(type_ref.type.get());
	if (type_ref.is_optional)
	{
		std::array<llvm::Type*, 2> struct_fields { llvm::Type::getInt8Ty(llvm_mod->getContext()), base_type };
		return llvm::StructType::get(llvm_mod->getContext(), llvm::makeArrayRef(struct_fields));
	}

	return base_type;
}

llvm::FunctionType* code_gen::code_gen::get_llvm_function_type(ir::ast::statement::function_declaration* func_def)
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

llvm::Function* code_gen::code_gen::get_or_declare_function(const std::string& symbol, ir::ast::statement::function_declaration* def_stat)
{
	std::string name;
	llvm::GlobalValue::LinkageTypes linkage;

	// TODO: maybe add destructors?
	// TODO: only one constructor per module? done i think
	bool is_constructor = def_stat->attributes.find("constructor") != def_stat->attributes.cend();
	bool is_extern = dynamic_cast<ir::ast::statement::extern_definition*>(def_stat) != nullptr;

	// TODO: test extern with @export or @constructor :KEKW:
	// TODO: add type name to function name
	if ((is_constructor && mod.is_root)
		|| def_stat->attributes.find("export") != def_stat->attributes.cend())
	{
		if (is_extern)
		{
			throw exception(def_stat->range.start, "exporting extern function is not allowed");
		}

		linkage = llvm::GlobalValue::ExternalLinkage;
		name = mod.relative_path + '@' + symbol;
	}
	else
	{
		linkage = is_extern ? llvm::GlobalValue::ExternalLinkage : llvm::GlobalValue::InternalLinkage;
		name = symbol;
	}

	auto function = llvm_mod->getFunction(symbol);
	if (!function)
	{
		llvm::FunctionType* func_type = get_llvm_function_type(def_stat);
		function = llvm::Function::Create(func_type, linkage, name, *llvm_mod);
	}

	return function;
}

code_gen::code_gen::code_gen(std::unordered_map<ir::types::type_descriptor*, llvm::Type*>& type_map, llvm::LLVMContext& context, ir::ast::module& root, const std::string& target_triple) :
	mod(root),
	type_map(type_map),
	llvm_mod(std::make_shared<llvm::Module>(root.relative_path, context)),
	builder(context)
{
	llvm_mod->setTargetTriple(target_triple);
	data_layout = std::make_unique<llvm::DataLayout>(llvm_mod.get());
}

std::shared_ptr<llvm::Module> code_gen::code_gen::gen_code()
{
	parser::symbol_collector collector;
	mod.body->visit(&collector);

	parser::variable_resolver variable_resolver{ collector.collected };
	mod.body->visit(&variable_resolver);

	code_gen_visitor gen{ *this };

	bool constructor_defined = false;
	for (auto [symbol, func_def] : collector.collected)
	{	
		llvm::GlobalValue::LinkageTypes linkage;
		auto function = get_or_declare_function(symbol, func_def);
		if (!function->empty())
		{
			throw exception(func_def->range.start, "function '" + func_def->name + "' already defined");
		}

		if (dynamic_cast<ir::ast::statement::extern_definition*>(func_def))
		{
			continue;
		}

		llvm::BasicBlock *basic_block = llvm::BasicBlock::Create(llvm_mod->getContext(), "entry", function);
		builder.SetInsertPoint(basic_block);

		func_def->visit(&gen);

		if (basic_block->empty() || !llvm::isa<llvm::ReturnInst>(basic_block->back()))
		{
			builder.CreateRetVoid();
		}

		llvm::verifyFunction(*function);
	}

	return llvm_mod;
}