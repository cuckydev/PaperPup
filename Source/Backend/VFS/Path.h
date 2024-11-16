#pragma once

#include <filesystem>

#include "Util/String.h"

#include <string>

namespace PaperPup::VFS
{

class Path
{
private:
	std::string data;

public:
	Path(const char *path)
	{
		data = path;
	}

	Path(const std::filesystem::path &name)
	{
		std::filesystem::path normal = name.lexically_normal();
		if (normal.is_absolute())
			return;

		for (const auto &part : normal)
		{
			if (part == "..")
				return;
			if (part == ".")
				continue;

			if (!data.empty())
				data.push_back('/');

			auto *part_str = part.c_str();
#ifdef WIN32
			auto part_utf8 = Util::UCS2ToUTF8(part_str);
			data.append(part_utf8);
#else
			data.append(part_str);
#endif
		}
	}

	Path(const Path &path) = default;
	Path(Path &&path) = default;

	const std::string &String() const
	{
		return data;
	}

	Path &operator=(const Path &path) = default;
	Path &operator=(Path &&path) = default;

	operator bool() const
	{
		return !data.empty();
	}
};

}
