/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "SPU.h"

namespace System
{
	namespace SPU
	{
		//SPU class
		//XA interface
		bool SPU::XA_Play(std::shared_ptr<std::istream> stream)
		{
			//Check if stream is valid
			if (stream == nullptr || stream->good() == false)
			{
				XA_Stop();
				return false;
			}
			
			//Lock mutex
			Mutex_Lock();
			
			//Set XA state
			xa_stream = stream;
			xa_filter_file = xa_filter_channel = 0;
			
			//Unlock mutex
			Mutex_Unlock();
			return false;
		}
		
		void SPU::XA_SetFilter(int file, int channel)
		{
			//Lock mutex
			Mutex_Lock();
			
			//Use given filter
			xa_filter_file = file;
			xa_filter_channel = channel;
			
			//Unlock mutex
			Mutex_Unlock();
		}
		
		void SPU::XA_Stop()
		{
			//Lock mutex
			Mutex_Lock();
			
			//Unlock mutex
			Mutex_Unlock();
		}
	}
}
