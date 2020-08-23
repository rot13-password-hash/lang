#pragma once

#include "position.h"

#include <llvm/Support/Error.h>
#include <llvm/Object/Error.h>
#include <llvm/Support/WithColor.h>

#include <filesystem>

namespace seam::compiler
{
	class error_info : public llvm::ErrorInfo<error_info>
	{
	public:
		static char ID;

		std::filesystem::path file;
		position pos;
		std::string msg;

		error_info(std::filesystem::path file, position pos, std::string msg) :
			file(std::move(file)), pos(pos), msg(std::move(msg)) {}

		void log(llvm::raw_ostream &OS) const override
		{
			OS << file.string() << ':' << pos.col << ':' << pos.line << ": ";
			llvm::WithColor::error(OS) << msg;
		}

		std::error_code convertToErrorCode() const override
		{
			return llvm::object::make_error_code(llvm::object::object_error::parse_failed);
		}
	};
}