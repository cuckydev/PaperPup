#include "Main.h"

#include "Backend/Core.h"
#include "Backend/VFS.h"
#include "Backend/Render.h"
#include "Backend/Audio.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <memory>

#include "Types/File.h"
#include "PS1/INT.h"

#include "PS1/VDF.h"
#include "PS1/TMD.h"
#include "PS1/VDFActor.h"
#include "PS1/Context.h"

#include "Util/String.h"

#include "PS1/Shader.h"

#include <SDL3/SDL_events.h>

#include "Types/Span.h"

#include "PS1/ADPCM.h"

#include "clownresampler.h"

#include "Script/Thread.h"

namespace PaperPup
{

static PS1::XADecode::XASectorOut xa_buffer;
static PS1::XADecode xa_decoder;
static uint64_t xa_sample_p = 0;

static ClownResampler_Precomputed precomputed;
static ClownResampler_HighLevel_State resampler;

static std::ifstream stream("C:/Users/CKDEV/Documents/DuckStation/isos/rapper/Image/S2/STAGE2.XA1", std::ios::binary);

static size_t ResamplerInputCallback(void *user_data, cc_s16l *buffer, size_t total_frames)
{
	(void)user_data;

	static char sector[0x920];
	for (size_t i = 0; i < total_frames; i++)
	{
		while (xa_sample_p >= xa_buffer.samples_count)
		{
			xa_sample_p -= xa_buffer.samples_count;
			while (1)
			{
				stream.read(sector, sizeof(sector));
				if (!stream)
					goto ItsOver;
				PS1::SubHeader &subheader = *reinterpret_cast<PS1::SubHeader *>(sector);
				if (subheader.file != 1 || subheader.channel != 1)
					continue;
				xa_decoder.DecodeXASector(Types::Span<const char, 0x920>(sector), xa_buffer);
				break;
			}
		}

		int16_t sample_l = xa_buffer.samples[xa_sample_p][0];
		int16_t sample_r = xa_buffer.samples[xa_sample_p][1];

		*buffer++ = sample_l;
		*buffer++ = sample_r;

		xa_sample_p += 1;
	}

	ItsOver:
	return total_frames;
}

typedef struct ResamplerCallbackData
{
	int16_t *output_pointer;
	size_t output_buffer_frames_remaining;
} ResamplerCallbackData;

static cc_bool ResamplerOutputCallback(void *user_data, const cc_s32f *frame, cc_u8f total_samples)
{
	ResamplerCallbackData *const callback_data = reinterpret_cast<ResamplerCallbackData*>(user_data);

	cc_u8f i;

	if (total_samples != 2)
		throw std::runtime_error("Invalid number of channels");

	/* Output the frame. */
	for (i = 0; i < total_samples; ++i)
	{
		cc_s32f sample;

		sample = frame[i];

		/* Clamp the sample to 16-bit. */
		if (sample > 0x7FFF)
			sample = 0x7FFF;
		else if (sample < -0x7FFF)
			sample = -0x7FFF;

		/* Push the sample to the output buffer. */
		*callback_data->output_pointer++ = static_cast<int16_t>(sample);
	}

	/* Signal whether there is more room in the output buffer. */
	return --callback_data->output_buffer_frames_remaining != 0;
}

// Game entry point
static void Main()
{
	Core::Backend::Instance();
	VFS::Backend::Instance();
	Render::Backend::Instance();
	auto &audio = Audio::Backend::Instance();
	(void)audio;

	ClownResampler_Precompute(&precomputed);
	ClownResampler_HighLevel_Init(&resampler, 2, 37800, audio.GetFrequency(), 44100);

	audio.SetMixCallback([](size_t frames)
	{
		static std::vector<std::array<int16_t, 2>> out;
		out.resize(frames);

		ResamplerCallbackData callback_data;

		callback_data.output_pointer = reinterpret_cast<int16_t*>(out.data());
		callback_data.output_buffer_frames_remaining = frames;

		/* Resample the decoded audio data. */
		ClownResampler_HighLevel_Resample(&resampler, &precomputed, ResamplerInputCallback, ResamplerOutputCallback, &callback_data);

		/* If there are no more samples left, then fill the remaining space in the buffer with 0. */
		memset(callback_data.output_pointer, 0, callback_data.output_buffer_frames_remaining * 2 * sizeof(int16_t));

		return reinterpret_cast<const int16_t *>(out.data());
	});

	auto window = Render::Backend::Instance().window;
	auto context = Render::Backend::Instance().context;

	SDL_GL_MakeCurrent(window, context);
	SDL_GL_SetSwapInterval(1);

	std::string lua_source = R"(
		print(`Thread.Wait(10) returns {Thread.Wait(10)}`)
	)";

	Script::Thread thread(lua_source);
	thread.SetContext({ .identity = Script::Context::Identity::UserScript });
	thread.Resume();

	// auto my_file = std::make_shared<MyFile>("C:/Users/CKDEV/Documents/DuckStation/isos/rapper/Image/S1/COMPO01.INT");
	// INT::INT int_file(my_file);

	/*
	const char *names[] = {
		"LA.TMD",
		"LAG.TMD",
		// "ON.TMD",
		"SET.TMD",
		"SET_BACK.TMD",
		"STAGE.TMD",
		"TABLE.TMD",
		"TOP.TMD",
		// "MA.TMD",
		// "KT.TMD",
		// "KTB.TMD",
		"WALL_B.TMD",
		"WALL_F.TMD",
		"WALL_L.TMD",
		"WALL_R.TMD",
		"WING_L.TMD",
		"WING_R.TMD",
		"DRUM.TMD",
		"KEYS.TMD",
	};
	
	auto tmd_models = std::make_unique<std::unique_ptr<TMD::Model>[]>(sizeof(names) / sizeof(names[0]));
	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++)
	{
		std::unique_ptr<Types::File> tmd_file(int_file.Open(names[i]));
		auto tmd_data = std::make_shared<TMD::Data>(*tmd_file);
		tmd_models[i] = std::make_unique<TMD::Model>(tmd_data);
	}

	PS1::Context ps1;
	for (auto &i : int_file.TIMs())
	{
		std::unique_ptr<Types::File> tim_file(int_file.Open(i.c_str()));
		ps1.LoadTIM(*tim_file);
	}

	auto &shader = PS1::Shader::Instance();

	glm::vec2 rotate(0.0f, 0.0f);
	*/

	while (1)
	{
		bool quit = false;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				quit = true;
				break;
			}
		}
		if (quit) break;

		// Init opengl 3.3 state for this frame
		int w = 0, h = 0;
		SDL_GetWindowSize(window, &w, &h);

		glViewport(0, 0, w, h);

		glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 3D SCENE TEST
		/*
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Enable backface culling
		glEnable(GL_CULL_FACE);

		// Bind vram texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ps1.vram_texture_id.Get());
		glUniform1i(glGetUniformLocation(shader.shader.program.Get(), "u_vram"), 0);

		// Setup projection matrix
		rotate.x += 0.01f;
		rotate.y -= 0.001f;
		if (rotate.y < 2.5f) rotate.y += 2.0f;
		glm::mat4 projection = camera(3000.0f, rotate, static_cast<float>(w) / static_cast<float>(h));
		glUniformMatrix4fv(glGetUniformLocation(shader.shader.program.Get(), "u_projection"), 1, GL_FALSE, &projection[0][0]);

		shader.shader.Bind();

		for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++)
		{
			tmd_models[i]->Draw();
		}
		*/

		SDL_GL_SwapWindow(window);
	}
}

}

// Platform specific entry point
#if defined(WIN32)
#include <Windows.h>

#ifdef UNICODE
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
#else
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pCmdLine, _In_ int nCmdShow)
#endif
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)pCmdLine;
	(void)nCmdShow;

	try
	{
		PaperPup::Main();
	}
	catch (std::exception &e)
	{
		auto ucs2_what = PaperPup::Util::UTF8ToUCS2(e.what());
		MessageBoxW(nullptr, ucs2_what.c_str(), L"Exception Caught", MB_OK | MB_ICONERROR);
	}
	return 0;
}

#endif

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	try
	{
		PaperPup::Main();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
