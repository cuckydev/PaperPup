#pragma once

#include <span>
#include <cstdint>

#include <SDL3/SDL_audio.h>

namespace PaperPup::Audio
{

class Backend
{
private:
	SDL_AudioStream *output_stream = nullptr;
	SDL_AudioDeviceID output_device = 0;

	uint32_t output_frequency = 0;

	typedef const int16_t *(*MixCallback)(size_t frames);

	MixCallback mix_callback = nullptr;

	static void SDLAudioCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);

private:
	Backend();
	~Backend();

public:
	static Backend &Instance();

	uint32_t GetFrequency() const;

	void SetMixCallback(MixCallback callback);

	void Lock();
	void Unlock();
};

}
