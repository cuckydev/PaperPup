/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Mixer.h"

#include <fstream>

namespace System
{
	namespace SPU
	{
		namespace Mixer
		{
			//Mixer resampler class
			void Resampler::Resample(int16_t *output, size_t frames, std::function<void(int16_t*, size_t)> input)
			{
				
			}
			
			//Mixer class
			//Mixer interface
			void Mixer::SetOutputFrequency(uint32_t frequency)
			{
				//Update output frequency
				output_frequency = frequency;
				
				//Update resamplers
				xa_resampler[0].SetResampleFrequency(output_frequency, xa_resampler[0].GetResampleFrequency());
				xa_resampler[1].SetResampleFrequency(output_frequency, xa_resampler[1].GetResampleFrequency());
			}
			
			void Mixer::Mix(int16_t *output, size_t frames)
			{
				
			}
			
			//XA interface
			void Mixer::XA_Play(std::istream *stream)
			{
				//Stop playing if bad stream
				if (stream == nullptr || stream->good() == false)
				{
					XA_Stop();
					return;
				}
				
				//Read XA data
				uint8_t init_file = 0, init_channel = 0, init_filter = 0;
				xa_channels.clear();
				
				while (!stream->eof())
				{
					//Read sector
					char sector[2336];
					stream->read(sector, 2336);
					
					//Use filter
					if (init_filter == 0)
					{
						init_file = sector[0];
						init_channel = sector[1];
						init_filter = 1;
					}
					
					//Check type
					if (sector[2] & (1 << 2))
					{
						//Get channel information
						XA::Channel *xa_channel;
						
						uint16_t filter = (sector[0] << 8) | sector[1];
						auto find_channel = xa_channels.find(filter);
						if (find_channel == xa_channels.end())
						{
							//Create new channel
							xa_channel = &xa_channels[filter];
							xa_channel->channel_coding = sector[3];
						}
						else
						{
							//Use given channel
							xa_channel = &find_channel->second;
						}
						
						//Convert audio data
						if ((xa_channel->channel_coding & XA::Coding::SampleBits) == XA::Coding::SampleBits_4)
						{
							if ((xa_channel->channel_coding & XA::Coding::Channels) == XA::Coding::Channels_Mono)
							{
								const uint8_t *datap = (const uint8_t*)(sector + 8);
								for (int s = 0; s < 18; s++)
								{
									for (int i = 0; i < 8; i++)
									{
										int16_t decode[28];
										xa_channel->channel_decoder[0].DecodeSector4(datap[4 + i], &datap[16], i, decode);
										xa_channel->channel_pcm.insert(xa_channel->channel_pcm.end(), &decode[0], &decode[28]);
									}
									datap += 128;
								}
							}
							else
							{
								throw "Stereo XA unsupported";
							}
						}
						else
						{
							throw "8-bit XA unsupported";
						}
					}
				}
				
				std::ofstream FUCKYOU("test.raw", std::ofstream::binary);
				FUCKYOU.write((char*)xa_channels[0x0101].channel_pcm.data(), xa_channels[0x0101].channel_pcm.size() * 2);
				
				//Set XA state
				XA_SetFilter(init_file, init_channel);
			}
			
			void Mixer::XA_SetFilter(uint8_t file, uint8_t channel)
			{
				//Use given filter
				xa_filter = (file << 8) | channel;
				
				//Initialize XA state
				auto find_channel = xa_channels.find(xa_filter);
				if (find_channel != xa_channels.end())
				{
					//Set resample frequency
					uint32_t channel_frequency = ((uint8_t)find_channel->second.channel_coding & XA::Coding::SampleRate) == XA::Coding::SampleRate_2 ? 37800 : 18900;
					xa_resampler[0].SetResampleFrequency(output_frequency, channel_frequency);
					xa_resampler[1].SetResampleFrequency(output_frequency, channel_frequency);
				}
			}
			
			void Mixer::XA_Stop()
			{
				//Set XA state
				xa_playing = false;
			}
		}
	}
}
