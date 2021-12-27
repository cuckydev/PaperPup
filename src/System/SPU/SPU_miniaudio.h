/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "SPU.h"

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_GENERATION

#include "miniaudio.h"

namespace System
{
	namespace SPU
	{
		//SPU miniaudio class
		class SPU_miniaudio : public SPU
		{
			private:
				//Miniaudio state
				ma_context miniaudio_context;
				ma_mutex miniaudio_mutex;
				ma_device miniaudio_device;
				
			public:
				//Constructor and destructor
				SPU_miniaudio();
				~SPU_miniaudio() override;
				
				//Miniaudio data callback
				void DataCallback(ma_device *device, void *output_buffer_void, const void *input_buffer, ma_uint32 frames_to_do);
				
			private:
				//Mutex interface
				void Mutex_Lock() override;
				void Mutex_Unlock() override;
		};
	}
}
