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
	namespace Frontend
	{
		// Frontend class
		class Frontend
		{
			protected:
				// Window state
				std::string window_title;
				unsigned int window_width = 0, window_height = 0;
				
			public:
				// Constructor and destructor
				virtual ~Frontend() {}
				
				// Window interface
				virtual void Window_Set(std::string title, unsigned int width, unsigned int height) = 0;
				
				std::string Window_GetTitle() { return window_title; }
				unsigned int Window_GetWidth() { return window_width; }
				unsigned int Window_GetHeight() { return window_height; }
				
				virtual void Window_SwapBuffers() = 0;
				
				// Input interface
				virtual void Input_HandleEvents() = 0;
				virtual bool Input_ShouldClose() = 0;
		};
	}
}
