/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "GPU_GL.h"

#include <algorithm>
#include <iterator>

#include <iostream>

//GL shaders
static const char *gl_shader_vert = R"(
#version 150 core

in vec3 v_position;
in vec2 v_uv;
in vec4 v_colour;

out vec2 f_uv;
out vec4 f_colour;

uniform mat4 u_projection;

void main()
{
	//Pass vertex data to fragment shader
	f_uv = v_uv;
	f_colour = v_colour;
	
	//Project vertex onto screen
	gl_Position = u_projection * vec4(v_position, 1.0);
}
)";

static const char *gl_shader_frag_flat = R"(
#version 150 core

in vec2 f_uv;
in vec4 f_colour;

out vec4 o_colour;

void main()
{
	//Use vertex colouring
	o_colour = f_colour;
}
)";

static const char *gl_shader_frag_16 = R"(
#version 150 core

in vec2 f_uv;
in vec4 f_colour;

out vec4 o_colour;

uniform usampler2D u_texture;

void main()
{
	//Get texel
	ivec2 texel_coord = ivec2(int(f_uv.x), int(f_uv.y));
	uvec4 texel = texelFetch(u_texture, texel_coord, 0);
	uint subtexel = (texel.r >> ((uint(texel_coord.x) & 1u) << 4)) & 0x0000FFFFu;
	subtexel = (subtexel >> 8) | (subtexel << 8);
	
	//Get RGBA colour from texel
	float r = float((subtexel & 0x7C00u) >> 10) / 32.0;
	float g = float((subtexel & 0x03E0u) >> 5)  / 32.0;
	float b = float((subtexel & 0x001Fu))       / 32.0;
	
	//Write texel
	if ((subtexel & 0x8000u) != 0u)
	{
		//Enable blending
		o_colour = vec4(r, g, b, 1.0) * f_colour;
	}
	else
	{
		//No blending
		if ((r + g + b) == 0.0)
			discard;
		o_colour = vec4(r, g, b, 1.0) * vec4(f_colour.rgb, 1.0);
	}
}
)";

namespace System
{
	namespace GPU
	{
		//GPU GL class
		//Constructor and destructor
		GPU_GL::GPU_GL()
		{
			//Generate VRAM texture
			glGenTextures(1, &vram_texture);
			
			//Set texture parameters
			glBindTexture(GL_TEXTURE_2D, vram_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, VRAM_TEXW / 2, VRAM_TEXH, 0, GL_R8UI, GL_UNSIGNED_INT, NULL);
			
			//Compile shaders
			shader_flat.Compile(gl_shader_vert, gl_shader_frag_flat);
			shader_16.Compile(gl_shader_vert, gl_shader_frag_16);
		}
		
		GPU_GL::~GPU_GL()
		{
			//Destroy VRAM texture
			glDeleteTextures(1, &vram_texture);
		}
		
		//Screen interface
		void GPU_GL::Screen_Set(unsigned int width, unsigned int height)
		{
			//Use given dimensions
			screen_width = width;
			screen_height = height;
		}
		
		//VRAM interface
		void GPU_GL::VRAM_Fill(uint16_t f, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			//Drop invalid coordinates
			if (w > VRAM_TEXW || x > (VRAM_TEXW - w) || h > VRAM_TEXH || y > (VRAM_TEXH - h))
				return;
			
			//Fill to VRAM
			uint16_t *dst = &vram[y][x];
			for (unsigned int cy = 0; cy < h; cy++)
			{
				std::fill(dst, dst + w, f);
				dst += VRAM_TEXW;
			}
			
			//Upload changes to VRAM texture
			VRAM_Upload(x, y, w, h);
		}
		
		void GPU_GL::VRAM_Write(const uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			//Drop invalid coordinates
			std::cout << "W " << (int)x << ", " << (int)y << ", " << (int)w << ", " << (int)h << std::endl;
			if (w > VRAM_TEXW || x > (VRAM_TEXW - w) || h > VRAM_TEXH || y > (VRAM_TEXH - h))
				return;
			
			//Read from data to VRAM
			uint16_t *dst = &vram[y][x];
			for (unsigned int cy = 0; cy < h; cy++)
			{
				std::copy(data, data + w, dst);
				dst += VRAM_TEXW;
				data += w;
			}
			
			//Upload changes to VRAM texture
			VRAM_Upload(x, y, w, h);
		}
		
		void GPU_GL::VRAM_Read(uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			//Drop invalid coordinates
			std::cout << "R " << (int)x << ", " << (int)y << ", " << (int)w << ", " << (int)h << std::endl;
			if (w > VRAM_TEXW || x > (VRAM_TEXW - w) || h > VRAM_TEXH || y > (VRAM_TEXH - h))
				return;
			
			//Read from VRAM to data
			const uint16_t *src = &vram[y][x];
			for (unsigned int cy = 0; cy < h; cy++)
			{
				std::copy(src, src + w, data);
				src += VRAM_TEXW;
				data += w;
			}
		}
		
		//Internal VRAM interface
		void GPU_GL::VRAM_Upload(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			//Copy 32-bit (two pixel) area
			w = (w + 1) >> 1;
			
			const uint8_t *src = (uint8_t*)(&vram[y][x & ~1]);
			uint32_t *data = new uint32_t[w * h];
			uint32_t *datap = data;
			
			for (unsigned int cy = 0; cy < h; cy++)
			{
				const uint8_t *srce = src + (VRAM_TEXW << 1);
				for (unsigned int cx = 0; cx < w; cx++)
				{
					*datap++ = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | (src[3] << 0);
					src += 4;
				}
				src = srce;
			}
			
			//Copy CPU VRAM to texture
			glBindTexture(GL_TEXTURE_2D, vram_texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_UNSIGNED_INT, (const void*)data);
			delete[] data;
		}
	}
}
