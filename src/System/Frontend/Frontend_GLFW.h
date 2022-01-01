/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "Frontend.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace System
{
	namespace Frontend
	{
		//Frontend GLFW class
		class Frontend_GLFW : public Frontend
		{
			private:
				//GLFW window
				GLFWwindow *glfw_window = nullptr;
				
			public:
				//Constructor and destructor
				Frontend_GLFW();
				~Frontend_GLFW() override;
				
				//Frontend interface
				void SetWindow(std::string window_title, unsigned int window_width, unsigned int window_height) override;
				
				void SwapBuffers() override;
				
				void HandleEvents() override;
				bool ShouldClose() override;
		};
	}
}
