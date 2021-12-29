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
				throw "[System::SPU::SPU_miniaudio::SPU_miniaudio] Failed to initialize miniaudio context";
			
			//Create miniaudio device
			ma_device_config config = ma_device_config_init(ma_device_type_playback);
			config.playback.pDeviceID = NULL;
			config.playback.format = ma_format_s16;
			config.playback.channels = 2;
			config.sampleRate = 0;
			config.noPreSilencedOutputBuffer = MA_TRUE;
			config.performanceProfile = ma_performance_profile_low_latency;
			config.dataCallback = [](ma_device *device, void *output_buffer_void, const void *input_buffer, ma_uint32 frames_to_do)
			{ ((SPU_miniaudio*)device->pUserData)->DataCallback(device, output_buffer_void, input_buffer, frames_to_do); };
			config.pUserData = (void*)this;
			
			if (ma_device_init(&miniaudio_context, &config, &miniaudio_device) != MA_SUCCESS)
				throw "[System::SPU::SPU_miniaudio::SPU_miniaudio] Failed to create miniaudio device";
			
			//Setup mixer
			mixer.SetOutputFrequency(miniaudio_device.sampleRate);
			
			//Create miniaudio mutex and start device
			if (ma_mutex_init(&miniaudio_mutex) != MA_SUCCESS)
				throw "[System::SPU::SPU_miniaudio::SPU_miniaudio] Failed to create miniaudio mutex";
			
			ma_device_start(&miniaudio_device);
		}
		
		SPU_miniaudio::~SPU_miniaudio()
		{
			//Deinitialize miniaudio
			ma_device_stop(&miniaudio_device);
			
			ma_device_uninit(&miniaudio_device);
			ma_mutex_uninit(&miniaudio_mutex);
			ma_context_uninit(&miniaudio_context);
		}
		
		//Miniaudio data callback
		void SPU_miniaudio::DataCallback(ma_device *device, void *output_buffer_void, const void *input_buffer, ma_uint32 frames_to_do)
		{
			(void)device;
			(void)input_buffer;
			
			//Run SPU mixer
			ma_mutex_lock(&miniaudio_mutex);
			mixer.Mix((int16_t*)output_buffer_void, frames_to_do);
			ma_mutex_unlock(&miniaudio_mutex);
		}
		
		//Mutex interface
		void SPU_miniaudio::Mutex_Lock()
		{
			//Lock mutex
			ma_mutex_lock(&miniaudio_mutex);
		}
		
		void SPU_miniaudio::Mutex_Unlock()
		{
			//Unlock mutex
			ma_mutex_unlock(&miniaudio_mutex);
		}
	}
}
