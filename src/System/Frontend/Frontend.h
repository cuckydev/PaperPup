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
		//Frontend class
		class Frontend
		{
			protected:
				//Window state
				std::string window_title;
				unsigned int window_width = 0, window_height = 0;
				
			public:
				//Constructor and destructor
				virtual ~Frontend() {}
				
				//Frontend interface
				virtual void SetWindow(std::string window_title, unsigned int window_width, unsigned int window_height) = 0;
				
				std::string GetTitle() { return window_title; }
				unsigned int GetWidth() { return window_width; }
				unsigned int GetHeight() { return window_height; }
				
				virtual void SwapBuffers() = 0;
				
				virtual void HandleEvents() = 0;
				virtual bool ShouldClose() = 0;
		};
	}
}
