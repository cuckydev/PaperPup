#pragma once

#include "glad/glad.h"
#ifdef APIENTRY
#undef APIENTRY
#endif

#include <SDL3/SDL_video.h>

namespace PaperPup::Render
{

class Backend
{
public:
	SDL_Window *window = nullptr;
	SDL_GLContext context = nullptr;

private:
	Backend();
	~Backend();

public:
	static Backend &Instance();
};

}
