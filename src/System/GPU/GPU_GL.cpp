/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "GPU_GL.h"

#include "glad/glad.h"

namespace System
{
	namespace GPU
	{
		//GPU GL class
		//Constructor and destructor
		GPU_GL::GPU_GL()
		{
			
		}
		
		GPU_GL::~GPU_GL()
		{
			
		}
		
		//Screen interface
		void GPU_GL::SetScreen(unsigned int screen_width, unsigned int screen_height)
		{
			//Set GL viewport
			glViewport(0, 0, screen_width, screen_height);
		}
	}
}
