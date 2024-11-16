#pragma once

#include "Backend/VFS/DataSource/DataSource.h"

#include <filesystem>

namespace PaperPup::VFS::DataSource
{

class Folder : public DataSource
{
private:
	std::filesystem::path folder_path;

public:
	Folder(const std::filesystem::path &path);
	~Folder() override;

	Types::File *Open(const Path &name) override;
};

}
