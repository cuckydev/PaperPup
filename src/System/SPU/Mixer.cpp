/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Mixer.h"

#include <algorithm>
#include <iterator>
#include <fstream>

#include "Main.h"

#define CLOWNRESAMPLER_IMPLEMENTATION
#include "clownresampler.h"

namespace System
{
	namespace SPU
	{
		namespace Mixer
		{
			// Mixer class
			// Mixer interface
			void Mixer::SetOutputFrequency(uint32_t frequency)
			{
				// Update output frequency
				output_frequency = frequency;
				
				// Update resamplers
				ClownResampler_HighLevel_Init(&xa_resampler, 2, xa_frequency, output_frequency);
			}
			
			void Mixer::Mix(int16_t *output, size_t frames)
			{
				// Allocate buffers
				int16_t *outputp;
				
				int32_t *buffer = new int32_t[frames << 1]{};
				int32_t *bufferp;
				
				short *resample_buffer = new short[frames << 1]{};
				short *resample_bufferp;
				
				// Write XA data
				if (xa_playing)
				{
					// Play current filtered channel
					auto find_channel = xa_channels.find(xa_filter);
					if (find_channel != xa_channels.end())
					{
						// Resample XA
						struct XA_Resample_Userdata
						{
							std::vector<int16_t> *pcm;
							size_t *xa_pos;
						};
						
						XA_Resample_Userdata userdata = {
							&find_channel->second.channel_pcm,
							&xa_pos
						};
						
						ClownResampler_HighLevel_Resample(&xa_resampler, resample_buffer, frames, [](void *void_userdata, short *buffer, size_t frames)
						{
							// Get userdata
							XA_Resample_Userdata *userdata = (XA_Resample_Userdata*)void_userdata;
							
							// Get number of frames to copy
							int16_t *input = (int16_t*)buffer;
							size_t samples = frames << 1;
							
							size_t sample_pos = (*userdata->xa_pos) << 1;
							
							int16_t *pcm_data = userdata->pcm->data();
							size_t pcm_size = userdata->pcm->size();
							
							size_t samples_left;
							if (sample_pos < pcm_size)
							{
								samples_left = pcm_size - sample_pos;
								if (samples_left > samples)
									samples_left = samples;
							}
							else
							{
								samples_left = 0;
							}
							
							// Copy samples
							std::copy(pcm_data + sample_pos, pcm_data + sample_pos + samples_left, input);
							
							frames = samples_left >> 1;
							*userdata->xa_pos += frames;
							
							return frames;
						}, (void*)&userdata);
						
						// Mix resampled XA
						bufferp = buffer;
						resample_bufferp = resample_buffer;
						for (size_t i = 0; i < (frames << 1); i++)
							*bufferp++ += *resample_bufferp++;
					}
				}
				
				// Clip to 16-bit output
				outputp = output;
				bufferp = buffer;
				
				for (size_t i = 0; i < frames; i++)
				{
					// Write left sample
					if (*bufferp < -0x7FFF)
						*outputp++ = -0x7FFF;
					else if (*bufferp > 0x7FFF)
						*outputp++ = 0x7FFF;
					else
						*outputp++ = *bufferp;
					bufferp++;
					
					// Write right sample
					if (*bufferp < -0x7FFF)
						*outputp++ = -0x7FFF;
					else if (*bufferp > 0x7FFF)
						*outputp++ = 0x7FFF;
					else
						*outputp++ = *bufferp;
					bufferp++;
				}
				
				// Delete buffers
				delete[] buffer;
				delete[] resample_buffer;
			}
			
			// XA interface
			void Mixer::XA_Load(std::istream &stream)
			{
				// Stop playing if bad stream
				if (stream.good() == false)
				{
					XA_Stop();
					return;
				}
				
				// Read XA data
				uint8_t init_file = 0, init_channel = 0, init_filter = 0;
				xa_channels.clear();
				
				while (!stream.eof())
				{
					// Read sector
					char sector[2336];
					stream.read(sector, 2336);
					
					// Use filter
					if (init_filter == 0)
					{
						init_file = sector[0];
						init_channel = sector[1];
						init_filter = 1;
					}
					
					// Check type
					if (sector[2] & (1 << 2))
					{
						// Get channel information
						XA::Channel *xa_channel;
						
						uint16_t filter = (sector[0] << 8) | sector[1];
						auto find_channel = xa_channels.find(filter);
						if (find_channel == xa_channels.end())
						{
							// Create new channel
							xa_channel = &xa_channels[filter];
							xa_channel->channel_coding = sector[3];
						}
						else
						{
							// Use given channel
							xa_channel = &find_channel->second;
						}
						
						// Convert audio data
						if ((xa_channel->channel_coding & XA::Coding::SampleBits) == XA::Coding::SampleBits_4)
						{
							if ((xa_channel->channel_coding & XA::Coding::Channels) == XA::Coding::Channels_Mono)
							{
								// Decode block by block
								const uint8_t *datap = (const uint8_t*)(sector + 8);
								for (int s = 0; s < 18; s++)
								{
									// Decode ADPCM
									int16_t decode[8 * 28];
									int16_t *decodep = decode;
									for (int i = 0; i < 8; i++)
									{
										xa_channel->channel_decoder[0].DecodeSector4(datap[4 + i], &datap[16], i, decodep);
										decodep += 28;
									}
									datap += 128;
									
									// Write to PCM
									decodep = decode;
									
									size_t pcmsize = xa_channel->channel_pcm.size();
									xa_channel->channel_pcm.resize(pcmsize + (8 * 28 * 2));
									int16_t *pcmp = &xa_channel->channel_pcm[pcmsize];
									
									for (int i = 0; i < (8 * 28); i++)
									{
										*pcmp++ = *decodep;
										*pcmp++ = *decodep++;
									}
								}
							}
							else
							{
								// Decode block by block
								const uint8_t *datap = (const uint8_t*)(sector + 8);
								for (int s = 0; s < 18; s++)
								{
									// Decode ADPCM
									int16_t decode[8 * 28];
									int16_t *decodel = decode;
									int16_t *decoder = decode + (4 * 28);
									
									for (int i = 0; i < 8; i += 2)
									{
										xa_channel->channel_decoder[0].DecodeSector4(datap[4 + i], &datap[16], i, decodel);
										xa_channel->channel_decoder[1].DecodeSector4(datap[5 + i], &datap[16], i | 1, decoder);
										decodel += 28;
										decoder += 28;
									}
									datap += 128;
									
									// Write to PCM
									decodel = decode;
									decoder = decode + (4 * 28);
									
									size_t pcmsize = xa_channel->channel_pcm.size();
									xa_channel->channel_pcm.resize(pcmsize + (8 * 28));
									int16_t *pcmp = &xa_channel->channel_pcm[pcmsize];
									
									for (int i = 0; i < (4 * 28); i++)
									{
										*pcmp++ = *decodel++;
										*pcmp++ = *decoder++;
									}
								}
							}
						}
						else
						{
							throw PaperPup::Exception("8-bit XA unsupported");
						}
					}
				}
				
				// Set XA state
				xa_playing = false;
				xa_pos = 0;
				XA_SetFilter(init_file, init_channel);
			}
			
			void Mixer::XA_Play()
			{
				// Set XA state
				xa_playing = true;
				ClownResampler_HighLevel_Init(&xa_resampler, 2, xa_frequency, output_frequency);
			}
			
			void Mixer::XA_SetFilter(uint8_t file, uint8_t channel)
			{
				// Use given filter
				xa_filter = (file << 8) | channel;
				
				// Initialize XA state
				auto find_channel = xa_channels.find(xa_filter);
				if (find_channel != xa_channels.end())
				{
					// Set resample frequency
					xa_frequency = ((uint8_t)find_channel->second.channel_coding & XA::Coding::SampleRate) == XA::Coding::SampleRate_2 ? 37800 : 18900;
					ClownResampler_HighLevel_Init(&xa_resampler, 2, xa_frequency, output_frequency);
				}
			}
			
			void Mixer::XA_Stop()
			{
				// Set XA state
				xa_playing = false;
			}
		}
	}
}
