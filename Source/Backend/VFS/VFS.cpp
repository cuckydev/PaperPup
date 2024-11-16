#include "Backend/VFS.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include <iostream>

namespace PaperPup::VFS
{

Backend::Backend()
{
	// Get install directory
#ifdef WIN32
	// Win32 method using GetModuleFileName
	std::basic_string<TCHAR> module_dir(MAX_PATH, static_cast<TCHAR>(0));
	while (1)
	{
		DWORD chars = GetModuleFileName(nullptr, module_dir.data(), static_cast<DWORD>(module_dir.size()));
		if (chars < module_dir.size())
		{
			module_dir.resize(chars);
			break;
		}
		else
		{
			module_dir.resize(module_dir.size() * 2);
		}
	}

	install_dir = std::filesystem::path(module_dir).parent_path();
#endif
}

Backend::~Backend()
{
	
}


Backend &Backend::Instance()
{
	static Backend instance;
	return instance;
}

}
