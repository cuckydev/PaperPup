/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "GPU_GL.h"

namespace System
{
	namespace GPU
	{
		//GPU GLFW class
		class GPU_GLFW : public GPU_GL
		{
			private:
				//GLFW objects
				
				
			public:
				//Constructor and destructor
				GPU_GLFW();
				~GPU_GLFW() override;
				
				//Screen interface
				void SetScreen(std::string title, unsigned int width, unsigned int height, unsigned int scale) override;
		};
	}
}
