/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <cstdint>
#include <cstddef>

namespace System
{
	namespace SPU
	{
		//Mixer class
		class Mixer
		{
			private:
				//Mixer state
				uint32_t output_frequency;
				
			public:
				//Mixer interface
				void SetOutputFrequency(uint32_t frequency) { output_frequency = frequency; }
				
				void Mix(int16_t *output, size_t frames);
		};
	}
}
