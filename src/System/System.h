/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "GPU/GPU.h"
#include "CD/CD.h"
#include "SPU/SPU.h"

namespace System
{
	//System class
	class System
	{
		private:
			//System components
			GPU::GPU *gpu = nullptr;
			CD::CD *cd = nullptr;
			SPU::SPU *spu = nullptr;
			
			//System state
			bool running = true;
			
		public:
			//Constructor and destructor
			System();
			~System();
			
			//System interface
			void Shutdown() { running = false; }
			bool Running() { return running; }
			
			//Gets and sets
			GPU::GPU *GetGPU() { return gpu; }
			CD::CD *GetCD() { return cd; }
			SPU::SPU *GetSPU() { return spu; }
	};
}
