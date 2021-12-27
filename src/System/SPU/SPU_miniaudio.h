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
				ma_device miniaudio_device;
				ma_mutex miniaudio_mutex;
				
				//XA state
				std::shared_ptr<std::istream> xa_stream = nullptr;
				int xa_filter_file, xa_filter_channel;
				
			public:
				//Constructor and destructor
				SPU_miniaudio();
				~SPU_miniaudio() override;
				
				//XA interface
				bool XA_Play(std::shared_ptr<std::istream> stream) override;
				void XA_SetFilter(int file, int channel) override;
				void XA_Stop() override;
		};
	}
}
