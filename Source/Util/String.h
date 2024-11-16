#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include <string>
#include <stdexcept>

namespace PaperPup::Util
{

#ifdef WIN32

inline std::basic_string<WCHAR> UTF8ToUCS2(const char *utf8)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
	if (length == 0)
		throw std::runtime_error("MultiByteToWideChar failed");

	std::basic_string<WCHAR> ucs2(static_cast<size_t>(length) - 1, 0);
	if (MultiByteToWideChar(CP_UTF8, 0, utf8, -1, ucs2.data(), length) == 0)
		throw std::runtime_error("MultiByteToWideChar failed");

	return ucs2;
}

inline std::string UCS2ToUTF8(const WCHAR *ucs2)
{
	int length = WideCharToMultiByte(CP_UTF8, 0, ucs2, -1, nullptr, 0, nullptr, nullptr);
	if (length == 0)
		throw std::runtime_error("WideCharToMultiByte failed");

	std::string utf8(static_cast<size_t>(length) - 1, 0);
	if (WideCharToMultiByte(CP_UTF8, 0, ucs2, -1, utf8.data(), length, nullptr, nullptr) == 0)
		throw std::runtime_error("WideCharToMultiByte failed");

	return utf8;
}

#endif

}
