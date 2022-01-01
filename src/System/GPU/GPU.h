/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <string>

namespace System
{
	namespace GPU
	{
		//GPU class
		class GPU
		{
			protected:
				//Screen state
				unsigned int screen_width, screen_height, screen_scale;
				
			public:
				//Constructor and destructor
				virtual ~GPU() {}
				
				//Screen interface
				virtual void SetScreen(std::string title, unsigned int width, unsigned int height, unsigned int scale) = 0;
				
				unsigned int GetWidth() { return screen_width; }
				unsigned int GetHeight() { return screen_height; }
				unsigned int GetScale() { return screen_scale; }
		};
	}
}
