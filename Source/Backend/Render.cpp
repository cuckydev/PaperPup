#include "Backend/Render.h"

#include "Backend/Core.h"

#include "Types/Exceptions.h"

namespace PaperPup::Render
{

static void *GetProcAddressWrapper(const char *name)
{
	return reinterpret_cast<void*>(SDL_GL_GetProcAddress(name));
}

Backend::Backend()
{
	// Require SDL
	Core::Backend::Instance();

	// Request an OpenGL 3.3 context
	int context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
#ifndef NDEBUG
	context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Create window
	window = SDL_CreateWindow("PaperPup", 1920, 1080, SDL_WINDOW_OPENGL);
	if (window == nullptr)
		throw Types::SDLException();

	// Create context
	context = SDL_GL_CreateContext(window);
	if (context == nullptr)
		throw Types::SDLException();
	SDL_GL_MakeCurrent(window, context);

	// Load glad
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(GetProcAddressWrapper)))
		throw Types::RuntimeException("gladLoadGLLoader failed");
}

Backend::~Backend()
{
	if (context != nullptr)
		SDL_GL_DestroyContext(context);
	if (window != nullptr)
		SDL_DestroyWindow(window);
}

Backend &Backend::Instance()
{
	static Backend instance;
	return instance;
}

}
