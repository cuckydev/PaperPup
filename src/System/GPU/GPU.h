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
		// GPU constants
		static const unsigned int VRAM_PAGEW = 16;
		static const unsigned int VRAM_PAGEH = 2;
		static const unsigned int VRAM_TEXW = VRAM_PAGEW * 64;
		static const unsigned int VRAM_TEXH = VRAM_PAGEH * 256;
		
		static const unsigned int VRAM_CPAGEW = VRAM_PAGEW * 4;
		
		static const unsigned int TPAGE_NULL = VRAM_PAGEW * VRAM_PAGEH;
		
		// GPU enums
		enum Bpp
		{
			Index4,
			Index8,
			RGBX16
		};
		
		enum Blend
		{
			Mix,  // 50% Background + 50% Polygon
			Add,  // 100% Background + 100% Polygon
			Sub,  // 100% Background - 100% Polygon
			Add4, // 100% Background + 25% Polygon
			None  // 0% Background + 100% Polygon
		};
		
		// GPU structures
		struct Vertex
		{
			float x, y, z = 0.0f;
			uint8_t r = 128, g = 128, b = 128;
			uint8_t u = 0, v = 0;
		};
		
		struct Triangle
		{
			Vertex vertex[3];
			unsigned int tpage = TPAGE_NULL, cpage;
			Bpp bpp;
			Blend blend = Blend::None;
		};
		
		struct Quad
		{
			Vertex vertex[4];
			unsigned int tpage = TPAGE_NULL, cpage;
			Bpp bpp;
			Blend blend = Blend::None;
		};
		
		struct Sprite
		{
			float x, y, w, h;
			uint8_t r = 128, g = 128, b = 128;
			uint8_t u, v;
			unsigned int tpage = TPAGE_NULL, cpage;
			Bpp bpp;
		};
		
		// GPU class
		class GPU
		{
			protected:
				// Screen state
				unsigned int screen_width, screen_height;
				
			public:
				// Constructor and destructor
				virtual ~GPU() {}
				
				// Screen interface
				virtual void Screen_Set(unsigned int width, unsigned int height) = 0;
				
				unsigned int Screen_GetWidth() { return screen_width; }
				unsigned int Screen_GetHeight() { return screen_height; }
				
				// VRAM interface
				virtual void VRAM_Fill(uint16_t f, unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
				virtual void VRAM_Write(const uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
				virtual void VRAM_Read(uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
				
				// Render interface
				virtual void Render_Triangle(const Triangle &tri) = 0;
				
				void Render_Quad(const Quad &quad)
				{
					// Copy general polygon structure
					Triangle tri;
					tri.tpage = quad.tpage;
					tri.cpage = quad.cpage;
					tri.bpp = quad.bpp;
					tri.blend = quad.blend;
					
					// Draw vertices 1 2 and 3
					tri.vertex[0] = quad.vertex[0];
					tri.vertex[1] = quad.vertex[1];
					tri.vertex[2] = quad.vertex[2];
					Render_Triangle(tri);
					
					// Draw vertices 4 3 and 2 (Flipped from PlayStation (according to docs) for winding)
					tri.vertex[0] = quad.vertex[3];
					tri.vertex[1] = quad.vertex[2];
					tri.vertex[2] = quad.vertex[1];
					Render_Triangle(tri);
				}
				
				void Render_Sprite(const Sprite &sprite)
				{
					
				}
		};
	}
}
