#pragma once

namespace PaperPup::Core
{

class Backend
{
private:
	Backend();
	~Backend();

public:
	static Backend &Instance();
};

}
