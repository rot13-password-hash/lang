#pragma once

#include <string>
#include <vector>
#include <utility>
#include <memory>

namespace lang::compiler::parser
{
	struct type_desc
	{
		std::string name;

		virtual bool is_built_in() const
		{
			return false;
		}

		virtual bool is_function_type() const
		{
			return false;
		}

		virtual bool is_alias_type() const
		{
			return false;
		}

	protected:
		type_desc(std::string name) :
			name(std::move(name))
		{}
	};

	struct type_alias_desc : public type_desc
	{
		std::weak_ptr<type_desc> desc;

		bool is_alias_type() const override
		{
			return true;
		}

		type_alias_desc(std::string name, const std::shared_ptr<type_desc>& desc) : type_desc(std::move(name)),
			desc(desc)
		{}
	};

	template <typename Type>
	class built_in_type_desc : public type_desc
	{
		using value_type = Type;

	public:
		bool is_built_in() const override
		{
			return true;
		}

		built_in_type_desc(std::string name) : type_desc(std::move(name))
		{}
	};

	using void_t = built_in_type_desc<void>;
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
		std::shared_ptr<type_desc> type;
	};

	class user_defined_type_desc : public type_desc
	{
		std::vector<field> fields;
	};

	struct function_signature
	{

	};

	class function_type : type_desc
	{
		function_signature signature;

		bool is_function_type() const override
		{
			return true;
		}
	};
}