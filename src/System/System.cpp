/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "System.h"

#include "CD/CD_Standard.h"
#include "SPU/SPU_miniaudio.h"

namespace System
{
	//System class
	//Constructor and destructor
	System::System()
	{
		//Create components
		cd = new CD::CD_Standard();
		spu = new SPU::SPU_miniaudio();
	}
	
	System::~System()
	{
		//Delete components
		if (spu != nullptr)
			delete spu;
	}
}
