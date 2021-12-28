/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

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
				Mixer::Mixer mixer;
				
			public:
				//Constructor and destructor
				virtual ~SPU() {}
				
				//XA interface
				void XA_Load(std::istream *stream)
				{
					//Lock stream and call mixer XA play
					Mutex_Lock();
					mixer.XA_Load(stream);
					Mutex_Unlock();
				}
				
				void XA_Play()
				{
					//Lock stream and call mixer XA play
					Mutex_Lock();
					mixer.XA_Play();
					Mutex_Unlock();
				}
				void XA_SetFilter(uint8_t file, uint8_t channel)
				{
					//Lock stream and call mixer XA set filter
					Mutex_Lock();
					mixer.XA_SetFilter(file, channel);
					Mutex_Unlock();
				}
				void XA_Stop()
				{
					//Lock stream and call mixer XA stop
					Mutex_Lock();
					mixer.XA_Stop();
					Mutex_Unlock();
				}
				
			private:
				//Mutex interface
				virtual void Mutex_Lock() = 0;
				virtual void Mutex_Unlock() = 0;
		};
	}
}
