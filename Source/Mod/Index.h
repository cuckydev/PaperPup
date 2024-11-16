#pragma once

#include <filesystem>

namespace PaperPup::Mod
{

class Index
{
private:
	std::filesystem::path mod_dir;

	Index();
	~Index();

	void NewIndex(const std::filesystem::path &mod_path);

public:
	static Index &Instance();
};

}
