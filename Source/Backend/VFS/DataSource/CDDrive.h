#pragma once

#include "Backend/VFS/DataSource/DataSource.h"

namespace PaperPup::VFS::DataSource
{

class CDDrive
{
public:
	virtual ~CDDrive() {}

	static CDDrive &Instance();
};

}
