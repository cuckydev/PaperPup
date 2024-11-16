#include "Backend/Core.h"

#include "Types/Exceptions.h"

#include <SDL3/SDL.h>

namespace PaperPup::Core
{

Backend::Backend()
{
	SDL_SetHint(SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES, "512");

	// Initialize SDL
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO | SDL_INIT_HAPTIC))
		throw Types::SDLException();
}

Backend::~Backend()
{
	// Deinitialize SDL
	SDL_Quit();
}

Backend &Backend::Instance()
{
	static Backend instance;
	return instance;
}

}
