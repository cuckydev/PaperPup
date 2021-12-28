/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <functional>
#include <iostream>
#include <utility>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstddef>

#include "ADPCM.h"

namespace System
{
	namespace SPU
	{
		namespace Mixer
		{
			//XA types
			namespace XA
			{
				enum Coding
				{
					Channels        = (3 << 0),
					Channels_Mono   = (0 << 0),
					Channels_Stereo = (1 << 0),
					
					SampleRate   = (3 << 2),
					SampleRate_2 = (0 << 2), //37800hz
					SampleRate_1 = (1 << 2), //18900hz
					
					SampleBits   = (3 << 4),
					SampleBits_4 = (0 << 4),
					SampleBits_8 = (1 << 4),
				};
				
				struct Channel
				{
					//XA channel data
					uint8_t channel_coding;
					
					ADPCM::Decoder channel_decoder[2];
					std::vector<int16_t> channel_pcm;
				};
			}
			
			//Mixer resampler class
			class Resampler
			{
				private:
					//Resampler state
					int16_t resample_from = 0, resample_to = 0;
					uint32_t resample_frequency = 0;
					uint32_t resample_sub = 0, resample_inc = 0;
					
				public:
					//Resampler interface
					void SetResampleFrequency(uint32_t output_frequency, uint32_t channel_frequency) { resample_frequency = channel_frequency; resample_inc = ((uint64_t)channel_frequency << 16) / output_frequency; }
					uint32_t GetResampleFrequency() { return resample_frequency; }
					
					void Reset() { resample_from = resample_to = resample_sub = 0; }
					size_t Resample(int16_t *output, size_t samples, std::function<void(int16_t*, size_t)> input);
			};
			
			//Mixer class
			class Mixer
			{
				private:
					//Mixer state
					uint32_t output_frequency = 0;
					
					//XA state
					bool xa_playing = false;
					
					uint16_t xa_filter = 0x0000;
					std::unordered_map<uint16_t, XA::Channel> xa_channels;
					
					size_t xa_pos = 0;
					
					Resampler xa_resampler[2];
					
				public:
					//Mixer interface
					void SetOutputFrequency(uint32_t frequency);
					uint32_t GetOutputFrequency() { return output_frequency; }
					
					void Mix(int16_t *output, size_t frames);
					
					//XA interface
					void XA_Load(std::istream *stream);
					
					void XA_Play();
					void XA_SetFilter(uint8_t file, uint8_t channel);
					void XA_Stop();
			};
		}
	}
}
