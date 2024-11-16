#pragma once

#include <stdexcept>

#include <SDL3/SDL_error.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#endif

namespace PaperPup::Types
{

// Exception types
class RuntimeException : public std::runtime_error
{
public:
	RuntimeException(const char *msg) : std::runtime_error(msg) {}
	RuntimeException(const std::string &msg) : std::runtime_error(msg) {}
};

class SDLException : public RuntimeException
{
public:
	SDLException() : RuntimeException(SDL_GetError()) {}
};

#ifdef WIN32
class Win32Exception : public RuntimeException
{
private:
	static const char *ExceptionWhat()
	{
		static char buffer[0x100];
		buffer[0] = '\0';
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof(buffer), nullptr);
		return buffer;
	}

public:
	Win32Exception() : RuntimeException(ExceptionWhat()) {}
};
#endif

}
