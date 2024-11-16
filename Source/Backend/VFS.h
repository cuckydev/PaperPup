#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "Backend/VFS/DataSource/DataSource.h"

namespace PaperPup::VFS
{

class Backend
{
private:
	std::filesystem::path install_dir;

	std::unordered_map<std::string, std::shared_ptr<DataSource::DataSource>> mods;

	// 
	Backend();
	~Backend();

public:
	static Backend &Instance();

	std::string MountMod(std::shared_ptr<DataSource::DataSource> mod_source);
	void UnmountMod(std::string mod_name);
};

}
