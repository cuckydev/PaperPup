/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "SPU_miniaudio.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

namespace System
{
	namespace SPU
	{
		//SPU miniaudio class
		//Constructor and destructor
		SPU_miniaudio::SPU_miniaudio()
		{
			//Initialize miniaudio context
			if (ma_context_init(NULL, 0, NULL, &miniaudio_context) != MA_SUCCESS)
				throw "[System::Spu::SPU_miniaudio::SPU_miniaudio] Failed to initialize miniaudio context";
			
			//Create miniaudio device
			ma_device_config config = ma_device_config_init(ma_device_type_playback);
			config.playback.pDeviceID = NULL;
			config.playback.format = ma_format_unknown; //Use native format
			config.playback.channels = 0;               //Use native channel count
			config.sampleRate = 0;                      //Use native sample rate
			config.noPreSilencedOutputBuffer = MA_TRUE; //We will clear this buffer ourselves if needed
			config.dataCallback = NULL;
			config.pUserData = NULL;
			
			if (ma_device_init(&miniaudio_context, &config, &miniaudio_device) != MA_SUCCESS)
				throw "[System::Spu::SPU_miniaudio::SPU_miniaudio] Failed to create miniaudio device";
		}
		
		SPU_miniaudio::~SPU_miniaudio()
		{
			
		}
		
		//XA interface
		bool SPU_miniaudio::XA_Play(std::shared_ptr<std::istream> stream)
		{
			//Initialize XA state
			xa_stream = stream;
			xa_filter_file = xa_filter_channel = 0;
			
			//Start playing
			
			return false;
		}
		
		void SPU_miniaudio::XA_SetFilter(int file, int channel)
		{
			//Use given file and channel
			xa_filter_file = file;
			xa_filter_channel = channel;
		}
		
		void SPU_miniaudio::XA_Stop()
		{
			//Stop playing XA
		}
	}
}
