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
#include <cstdint>
#include <cstddef>

namespace System
{
	namespace SPU
	{
		namespace Resampler
		{
			//Resampler class
			template<size_t channels> class Resampler
			{
				private:
					//Resampler state
					int16_t resampler_from[channels] = {};
					int16_t resampler_to[channels] = {};
					
					uint32_t resampler_frequency = 0;
					uint64_t resampler_sub = 0, resampler_inc = 0;
					
				public:
					//Resampler interface
					void Reset() { for (size_t i = 0; i < channels; i++) { resampler_from[i] = 0; resampler_to[i] = 0; } resampler_sub = 0; }
					
					void SetResampleFrequency(uint32_t output_frequency, uint32_t frequency) { resampler_frequency = frequency; resampler_inc = ((uint64_t)resampler_frequency << 32) / output_frequency; }
					uint32_t GetResampleFrequency() { return resampler_frequency; }
					
					void Resample(int32_t *output, size_t frames, std::function<void(int16_t*, size_t)> input)
					{
						//Check state
						if (resampler_inc == 0 || frames == 0)
							return;
						
						//Get samples required to read
						uint64_t resampler_end = resampler_sub + resampler_inc * frames;
						
						size_t resampler_need = (resampler_end + 0xFFFFFFFFULL) >> 32;
						if (resampler_sub)
							resampler_need--;
						
						//Read samples
						int16_t *resampler_buffer = new int16_t[(resampler_need + 2) * channels];
						
						int16_t *bufferc = resampler_buffer;
						for (size_t i = 0; i < channels; i++)
						{
							bufferc[0] = resampler_from[i];
							bufferc[channels] = resampler_to[i];
							bufferc++;
						}
						
						input(resampler_buffer + (2 * channels), resampler_need);
						
						//Resample and mix
						int32_t *outputp = output;
						int16_t *bufferp = resampler_buffer;
						
						for (size_t j = 0; j < frames; j++)
						{
							//Write interpolated sample
							uint64_t resampler_subl = resampler_sub & 0xFFFFFFFFULL;
							uint64_t resampler_subi = 0x100000000ULL - resampler_subl;
							
							for (size_t i = 0; i < channels; i++)
							{
								int16_t ext_f = resampler_from[i];
								int16_t ext_t = resampler_to[i];
								*outputp++ = ((ext_f * resampler_subi) + ext_t * resampler_subl) >> 32;
							}
							
							//Increment sub pointer
							resampler_sub += resampler_inc;
							
							//Update buffer if stream not ending
							//TODO: should I do for condition here?
							if (resampler_sub & ~0xFFFFFFFFULL)
							{
								bufferp += (resampler_sub >> 32) * channels;
								resampler_sub &= 0xFFFFFFFFULL;
								
								int16_t *bufferc = bufferp;
								for (size_t i = 0; i < channels; i++)
								{
									resampler_from[i] = bufferc[0];
									resampler_to[i] = bufferc[channels];
									bufferc += 1;
								}
							}
						}
						
						//Free buffers
						delete[] resampler_buffer;
					}
			};
		}
	}
}
