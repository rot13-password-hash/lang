#include "type_analyzer.h"

#include <unordered_map>
#include <sstream>

#include "../../utils/exception.h"

namespace lang::compiler::parser
{
	class type_collector : public ir::ast::visitor
	{
		std::unordered_map<std::string, std::shared_ptr<ir::types::type_descriptor>>& type_map;

	public:
		type_collector(std::unordered_map<std::string, std::shared_ptr<ir::types::type_descriptor>>& type_map) :
			type_map(type_map) {}

		bool visit(ir::ast::node* node) override
		{
			return false;
		}

		bool visit(ir::ast::statement::restricted_block* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::alias_type_definition* node) override
		{
			if (type_map.find(node->alias_name) == type_map.cend())
			{
				const auto& target_type = std::get<ir::ast::type>(node->target_type);
				const auto aliased_it = type_map.find(target_type.name);
				if (aliased_it == type_map.cend())
				{
					std::stringstream error_message;
					error_message << "attempt to alias invalid type '" << target_type.name << "' as '" << node->alias_name << "'";
					throw exception(node->range.start, error_message.str());
				}
				type_map[node->alias_name] = std::make_shared<ir::types::alias_type_descriptor>(node->alias_name, aliased_it->second);
			}
			else
			{
				std::stringstream error_message;
				error_message << "attempt to redefine type '" << node->alias_name << "'";
				throw exception(node->range.start, error_message.str());
			}
			return false;
		}

		bool visit(ir::ast::statement::class_type_definition* node) override
		{
			if (type_map.find(node->name) == type_map.cend())
			{
				auto class_desc = std::make_shared<ir::types::class_type_descriptor>(node->name);

				for (const auto& field : node->fields)
				{
					const auto& field_type = std::get<ir::ast::type>(field.type_);
					auto field_type_desc_it = type_map.find(field_type.name);
					if (field_type_desc_it == type_map.cend())
					{
						std::stringstream error_message;
						error_message << "attempt to declare field '" << field.name << "' with invalid type '" << field_type.name << "'";
						throw exception(node->range.start, error_message.str());
					}
					class_desc->fields[field.name] = { field.name, { field_type_desc_it->second, field_type.is_optional } };
				}
				type_map[node->name] = class_desc;
			}
			else
			{
				std::stringstream error_message;
				error_message << "attempt to redefine type '" << node->name << "'";
				throw exception(node->range.start, error_message.str());
			}
			return true;
		}
	};

	class type_resolver : public ir::ast::visitor
	{
		std::unordered_map<std::string, std::shared_ptr<ir::types::type_descriptor>>& type_map;

		void resolve_type(const lang::compiler::position& pos, ir::ast::type_reference& type_ref)
		{
			auto& type = std::get<ir::ast::type>(type_ref);

			auto type_desc_it = type_map.find(type.name);
			if (type_desc_it == type_map.cend())
			{
				std::stringstream error_message;
				error_message << "attempt to use invalid type '" << type.name << '\'';
				throw exception(pos, error_message.str());
			}

			type_ref = ir::types::type_reference{ type_desc_it->second, type.is_optional };
		}
	public:
		bool visit(ir::ast::statement::function_definition* node)
		{
			for (auto& argument : node->arguments)
			{
				resolve_type(node->range.start, argument.type_);
			}

			resolve_type(node->range.start, node->return_type);
			return true;
		}

		bool visit(ir::ast::statement::alias_type_definition* node)
		{
			resolve_type(node->range.start, node->target_type);
			return true;
		}

		bool visit(ir::ast::statement::class_type_definition* node)
		{
			for (auto& field : node->fields)
			{
				resolve_type(node->range.start, field.type_);
			}

			return true;
		}

		type_resolver(std::unordered_map<std::string, std::shared_ptr<ir::types::type_descriptor>>& type_map) :
			type_map(type_map) {}
	};

	void type_analyzer::invoke_single(ir::ast::statement::restricted_block* root)
	{
		std::unordered_map<std::string, std::shared_ptr<ir::types::type_descriptor>> type_map
		{
			{ "void", std::make_unique<ir::types::built_in_type_descriptor<void>>("void") },
			{ "string", std::make_unique<ir::types::built_in_type_descriptor<std::string>>("string") },
			{ "bool", std::make_unique<ir::types::built_in_type_descriptor<bool>>("bool") },
			{ "i8", std::make_unique<ir::types::built_in_type_descriptor<std::int8_t>>("i8") },
			{ "i16", std::make_unique<ir::types::built_in_type_descriptor<std::int16_t>>("i16") },
			{ "i32", std::make_unique<ir::types::built_in_type_descriptor<std::int32_t>>("i32") },
			{ "i64", std::make_unique<ir::types::built_in_type_descriptor<std::int64_t>>("i64") },
			{ "u8", std::make_unique<ir::types::built_in_type_descriptor<std::uint8_t>>("u8") },
			{ "u16", std::make_unique<ir::types::built_in_type_descriptor<std::uint16_t>>("u16") },
			{ "u32", std::make_unique<ir::types::built_in_type_descriptor<std::uint32_t>>("u32") },
			{ "u64", std::make_unique<ir::types::built_in_type_descriptor<std::uint64_t>>("u64") },
			{ "f32", std::make_unique<ir::types::built_in_type_descriptor<float>>("f32") },
			{ "f64", std::make_unique<ir::types::built_in_type_descriptor<double>>("f64") }
		};

		type_collector collector{ type_map };
		root->visit(&collector);

		type_resolver resolver{ type_map };
		root->visit(&resolver);
	}
}