#pragma once

#include <source_location>
#include <format>

#include "Types/Exceptions.h"

namespace PaperPup::Log
{

inline void Assert(bool condition, std::source_location source = std::source_location::current())
{
	if (!condition)
		throw Types::RuntimeException(std::format("Assertion failed at {}:{} ({})", source.file_name(), source.line(), source.function_name()));
}

inline void Assert(bool condition, const std::string &message, std::source_location source = std::source_location::current())
{
	if (!condition)
		throw Types::RuntimeException(std::format("Assertion \"{}\" failed at {}:{} ({})", message, source.file_name(), source.line(), source.function_name()));
}

}
