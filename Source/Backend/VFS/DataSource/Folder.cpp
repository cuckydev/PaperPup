#include "Backend/VFS/DataSource/Folder.h"

#include <fstream>

namespace PaperPup::VFS::DataSource
{

class FolderFile : public Types::File
{
private:
	std::unique_ptr<char[]> data;

	Types::Span<char> GetSpan(std::ifstream &stream)
	{
		size_t size = static_cast<size_t>(stream.tellg());
		stream.seekg(0, std::ios::beg);

		data = std::make_unique<char[]>(size);
		stream.read(data.get(), static_cast<std::streamsize>(size));

		return Types::Span<char>(data.get(), size);
	}

public:
	FolderFile(std::ifstream &stream) : File(GetSpan(stream))
	{

	}
};

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
	std::ifstream file(folder_path / path, std::ios::binary | std::ios::ate);
	if (!file.is_open())
		return nullptr;

	return new FolderFile(file);
}

}
