#include "Backend/Audio.h"

#include "Backend/Core.h"

#include "Types/Exceptions.h"

#include <SDL3/SDL_hints.h>

#include <atomic>
#include <iostream>

namespace PaperPup::Audio
{

void Backend::SDLAudioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
	Backend *audio = reinterpret_cast<Backend *>(userdata);
	(void)audio;

	if (additional_amount > 0)
	{
		// Call the user mixer
		std::atomic_signal_fence(std::memory_order_acquire);

		if (audio->mix_callback != nullptr)
		{
			size_t frames = static_cast<size_t>(additional_amount) / sizeof(int16_t) / 2;
			SDL_PutAudioStreamData(stream, audio->mix_callback(frames), additional_amount);
		}
	}
}

Backend::Backend()
{
	// Require SDL
	Core::Backend::Instance();

	// Get the preferred audio format
	SDL_AudioSpec preferred = {};
	int sample_frames = 0;

	if (!SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &preferred, &sample_frames))
	{
		std::cout << "SDL_GetAudioDeviceFormat failed: " << SDL_GetError() << std::endl;
		std::cout << "Using default audio format" << std::endl;
		preferred.freq = 44100;
	}
	else if (preferred.freq <= 0)
	{
		std::cout << "SDL_GetAudioDeviceFormat returned invalid frequency" << std::endl;
		std::cout << "Using default audio format" << std::endl;
		preferred.freq = 44100;
	}

	// Create audio stream
	SDL_AudioSpec desired = {};

	desired.format = SDL_AUDIO_S16;
	desired.channels = 2;
	desired.freq = preferred.freq;

	output_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, SDLAudioCallback, this);
	if (output_stream == nullptr)
		throw Types::SDLException();

	SDL_AudioSpec obtained = {};
	if (!SDL_GetAudioStreamFormat(output_stream, nullptr, &obtained))
		throw Types::SDLException();
	
	output_frequency = static_cast<uint32_t>(obtained.freq);

	// output_frequency = static_cast<uint32_t>(desired.freq);

	// Now get the actual audio device and resume it
	output_device = SDL_GetAudioStreamDevice(output_stream);
	if (output_device == 0)
		throw Types::SDLException();

	SDL_ResumeAudioStreamDevice(output_stream);
}

Backend::~Backend()
{
	if (output_stream != nullptr)
	{
		SDL_PauseAudioStreamDevice(output_stream);
		SDL_DestroyAudioStream(output_stream);
	}
}

uint32_t Backend::GetFrequency() const
{
	return output_frequency;
}

void Backend::SetMixCallback(MixCallback callback)
{
	Lock();

	mix_callback = callback;
	std::atomic_signal_fence(std::memory_order_release);

	Unlock();
}

void Backend::Lock()
{
	SDL_LockAudioStream(output_stream);
}

void Backend::Unlock()
{
	SDL_UnlockAudioStream(output_stream);
}

Backend &Backend::Instance()
{
	static Backend instance;
	return instance;
}

}
