#include "Mod/Index.h"

#include "Backend/VFS.h"

#include "Script/Thread.h"
#include "Script/Table.h"

#include <iostream>

namespace PaperPup::Mod
{

Index::Index()
{
	mod_dir = VFS::Backend::Instance().GetInstallDir() / "Mods";

	for (const auto &entry : std::filesystem::directory_iterator(mod_dir))
	{
		if (entry.is_directory())
			NewIndex(entry.path());
	}
}

Index::~Index()
{

}

void Index::NewIndex(const std::filesystem::path &mod_path)
{
	// Check if index file exists
	std::filesystem::path index_path = mod_path / "Index.lua";
	if (!std::filesystem::exists(index_path))
		return;

	// Execute index file
	auto index_file = Types::StlFile(index_path);
	auto index_thread = Script::Thread("=" + mod_path.string(), index_file);
	index_thread.SetContext({ Script::Context::Identity::IndexScript });

	index_thread.Resume();

	Script::TableContext table_ctx(index_thread.GetState());

	Script::Table index_table = table_ctx.GetTable(-1);

	std::string mod_name = index_table.Get<std::string>("Name");
	std::cout << "Mod: " << mod_name << std::endl;
}

Index &Index::Instance()
{
	static Index instance;
	return instance;
}

}
