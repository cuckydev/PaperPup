/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <memory>
#include <iostream>
#include <cstddef>

#include "Mixer.h"

namespace System
{
	namespace SPU
	{
		//SPU class
		class SPU
		{
			protected:
				//Mixer
				Mixer mixer;
				
			private:
				//XA state
				std::shared_ptr<std::istream> xa_stream;
				int xa_filter_file, xa_filter_channel;
				
			public:
				//Constructor and destructor
				virtual ~SPU() {}
				
				//XA interface
				bool XA_Play(std::shared_ptr<std::istream> stream);
				void XA_SetFilter(int file, int channel);
				void XA_Stop();
				
			private:
				//Mutex interface
				virtual void Mutex_Lock() = 0;
				virtual void Mutex_Unlock() = 0;
		};
	}
}
