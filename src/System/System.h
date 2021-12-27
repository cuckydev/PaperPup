/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "SPU/SPU.h"

namespace System
{
	//System class
	class System
	{
		private:
			//System components
			SPU::SPU *spu = nullptr;
			
		public:
			//Constructor and destructor
			System();
			~System();
			
			//Gets and sets
			SPU::SPU *GetSPU() { return spu; }
	};
}
