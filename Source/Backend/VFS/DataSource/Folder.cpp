#include "Backend/VFS/DataSource/Folder.h"

#include <fstream>

namespace PaperPup::VFS::DataSource
{

Folder::Folder(const std::filesystem::path &path) : folder_path(path)
{

}

Folder::~Folder()
{

}

Types::File *Folder::Open(const Path &name)
{
	// Convert path to filesystem path
	std::filesystem::path path;
	
	const auto &string = name.String();

	const char *str_p = string.c_str();
	const char *str_from = str_p;
	
	for (; *str_p != '\0'; str_p++)
	{
		if (*str_p == '/')
		{
			path /= std::string(str_from, static_cast<size_t>(str_p - str_from));
			str_from = str_p + 1;
		}
	}

	// Open file
	std::ifstream file(folder_path / path, std::ios::binary);
	if (!file.is_open())
		return nullptr;

	return new Types::StlFile(file);
}

}
