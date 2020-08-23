#pragma once

#include <unordered_map>
#include <string>
#include <memory>

namespace seam::compiler::ir::types
{
	struct type_descriptor
	{
		std::string name;

		type_descriptor(std::string name) :
			name(std::move(name)) {}

		virtual ~type_descriptor() = default;
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
}
