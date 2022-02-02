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
		// Frontend GLFW class
		class Frontend_GLFW : public Frontend
		{
			private:
				// GLFW window
				GLFWwindow *glfw_window = nullptr;
				
			public:
				// Constructor and destructor
				Frontend_GLFW();
				~Frontend_GLFW() override;
				
				// Window interface
				void Window_Set(std::string title, unsigned int width, unsigned int height) override;
				
				void Window_SwapBuffers() override;
				
				void Input_HandleEvents() override;
				bool Input_ShouldClose() override;
		};
	}
}
