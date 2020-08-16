#include "type_analyzer.h"

#include <unordered_map>
#include <sstream>

#include "../../utils/exception.h"

namespace lang::compiler::parser
{
	struct type_descriptor
	{
		std::string name;

		type_descriptor(std::string name) :
			name(std::move(name)) {}
	};
	
	struct type_reference
	{
		std::shared_ptr<type_descriptor> type;
		bool is_optional;
	};
	
	struct field_descriptor
	{
		std::string name;
		type_reference type;
	};

	struct class_type_descriptor : type_descriptor
	{
		std::unordered_map<std::string, field_descriptor> fields;

		using type_descriptor::type_descriptor;
	};

	struct alias_type_descriptor : type_descriptor
	{
		std::shared_ptr<type_descriptor> aliased_type;

		alias_type_descriptor(std::string name, std::shared_ptr<type_descriptor> aliased_type) :
			type_descriptor(std::move(name)), aliased_type(std::move(aliased_type)) {}
	};
	
	template <typename T>
	struct built_in_type_descriptor : type_descriptor
	{
		using type_descriptor::type_descriptor;
	};

	struct type_collector : ir::ast::visitor
	{
		std::unordered_map<std::string, std::shared_ptr<type_descriptor>>& type_map;

		bool visit(ir::ast::node* node) override
		{
			return false;
		}

		bool visit(ir::ast::statement::top_level_block* node) override
		{
			return true;
		}

		bool visit(ir::ast::statement::alias_type_definition* node) override
		{
			if (type_map.find(node->alias_name) == type_map.cend())
			{
				const auto aliased_it = type_map.find(node->target_type.name);
				if (aliased_it == type_map.cend())
				{
					std::stringstream error_message;
					error_message << "attempt to alias invalid type '" << node->target_type.name << "' as '" << node->alias_name << "'";
					throw exception(node->range.start, error_message.str());
				}
				type_map[node->alias_name] = std::make_shared<alias_type_descriptor>(node->alias_name, aliased_it->second);
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
				auto class_desc = std::make_shared<class_type_descriptor>(node->name);

				for (const auto& field : node->fields)
				{
					auto field_type_desc_it = type_map.find(field.variable.type_.name);
					if (field_type_desc_it == type_map.cend())
					{
						std::stringstream error_message;
						error_message << "attempt to declare field '" << field.variable.name << "' with invalid type '" << field.variable.type_.name << "'";
						throw exception(node->range.start, error_message.str());
					}
					class_desc->fields[field.variable.name] = { field.variable.name, { field_type_desc_it->second, field.variable.type_.is_optional } };
				}
				type_map[node->name] = class_desc;
			}
			else
			{
				std::stringstream error_message;
				error_message << "attempt to redefine type '" << node->name << "'";
				throw exception(node->range.start, error_message.str());
			}
			return false;
		}

		type_collector(std::unordered_map<std::string, std::shared_ptr<type_descriptor>>& type_map) :
			type_map(type_map) {}
	};

	void type_analyzer::invoke_single(ir::ast::statement::top_level_block* root)
	{
		std::unordered_map<std::string, std::shared_ptr<type_descriptor>> type_map
		{
			{ "void", std::make_unique<built_in_type_descriptor<void>>("void") },
			{ "string", std::make_unique<built_in_type_descriptor<std::string>>("string") },
			{ "bool", std::make_unique<built_in_type_descriptor<bool>>("bool") },
			{ "i8", std::make_unique<built_in_type_descriptor<std::int8_t>>("i8") },
			{ "i16", std::make_unique<built_in_type_descriptor<std::int16_t>>("i16") },
			{ "i32", std::make_unique<built_in_type_descriptor<std::int32_t>>("i32") },
			{ "i64", std::make_unique<built_in_type_descriptor<std::int64_t>>("i64") },
			{ "u8", std::make_unique<built_in_type_descriptor<std::uint8_t>>("u8") },
			{ "u16", std::make_unique<built_in_type_descriptor<std::uint16_t>>("u16") },
			{ "u32", std::make_unique<built_in_type_descriptor<std::uint32_t>>("u32") },
			{ "u64", std::make_unique<built_in_type_descriptor<std::uint64_t>>("u64") },
			{ "f32", std::make_unique<built_in_type_descriptor<float>>("f32") },
			{ "f64", std::make_unique<built_in_type_descriptor<double>>("f64") }
		};

		type_collector collector{ type_map };
		root->visit(&collector);

	}
}