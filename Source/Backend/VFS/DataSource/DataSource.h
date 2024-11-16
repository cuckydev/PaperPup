#pragma once

#include "Types/File.h"

#include <filesystem>

#include "Backend/VFS/Path.h"

namespace PaperPup::VFS::DataSource
{

class DataSource
{
public:
	virtual ~DataSource() = default;

	virtual Types::File *Open(const Path &name) = 0;
};

}
