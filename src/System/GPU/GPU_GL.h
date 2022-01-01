/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "GPU.h"

namespace System
{
	namespace GPU
	{
		//GPU GL class
		class GPU_GL : public GPU
		{
			public:
				//Constructor and destructor
				GPU_GL();
				~GPU_GL() override;
				
				//Screen interface
				void SetScreen(unsigned int screen_width, unsigned int screen_height) override;
		};
	}
}
