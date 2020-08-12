#pragma once

#include <string>
#include <vector>
#include <utility>
#include <memory>

namespace lang::compiler::parser
{
	class type_desc
	{
		std::string name;

		virtual bool is_built_in() const
		{
			return false;
		}

	protected:
		type_desc(std::string name) :
			name(std::move(name)) {}

		type_desc() {}
	};

	template <typename Type>
	class built_in_type_desc : public type_desc
	{
		using value_type = Type;

	public:
		bool is_built_in() const
		{
			return true;
		}

		built_in_type_desc(std::string name) : type_desc(std::move(name))
		{}
	};

	using i32_t = built_in_type_desc<std::int32_t>;
	using u32_t = built_in_type_desc<std::uint32_t>;
	using i64_t = built_in_type_desc<std::int64_t>;
	using u64_t = built_in_type_desc<std::uint64_t>;
	using bool_t = built_in_type_desc<bool>;
	using f32_t = built_in_type_desc<float>;
	using f64_t = built_in_type_desc<double>;
	using string_t = built_in_type_desc<std::string>;

	struct field
	{
		std::string name;
		std::string type;
	};

	class user_defined_type_desc : type_desc
	{
		std::vector<field> fields;

	public:
		user_defined_type_desc(std::string name) : type_desc(std::move(name))
		{}	
	};
}