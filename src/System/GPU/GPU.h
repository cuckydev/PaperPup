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
#include <cstdint>

namespace System
{
	namespace GPU
	{
		//GPU constants
		static const unsigned int VRAM_PAGEW = 16;
		static const unsigned int VRAM_PAGEH = 2;
		static const unsigned int VRAM_TEXW = VRAM_PAGEW * 64;
		static const unsigned int VRAM_TEXH = VRAM_PAGEH * 256;
		
		//GPU class
		class GPU
		{
			protected:
				//Screen state
				unsigned int screen_width, screen_height;
				
			public:
				//Constructor and destructor
				virtual ~GPU() {}
				
				//Screen interface
				virtual void Screen_Set(unsigned int width, unsigned int height) = 0;
				
				unsigned int Screen_GetWidth() { return screen_width; }
				unsigned int Screen_GetHeight() { return screen_height; }
				
				//VRAM interface
				virtual void VRAM_Fill(uint16_t f, unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
				virtual void VRAM_Write(const uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
				virtual void VRAM_Read(uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
		};
	}
}
