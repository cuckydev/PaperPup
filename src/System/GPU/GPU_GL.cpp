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

// GL shaders
static const char *gl_shader_vert = R"(
#version 150 core

in vec3 v_position;
in vec2 v_uv;
in vec2 v_cuv;
in vec4 v_colour;

out vec2 f_uv;
out vec2 f_cuv;
out vec4 f_colour;

void main()
{
	// Pass vertex data to fragment shader
	f_uv = v_uv;
	f_cuv = v_cuv;
	f_colour = v_colour;
	
	// Project vertex onto screen
	gl_Position = vec4(v_position, 1.0f);
}
)";

static const char *gl_shader_frag_flat = R"(
#version 150 core

in vec2 f_uv;
in vec2 f_cuv;
in vec4 f_colour;

out vec4 o_colour;

void main()
{
	// Use vertex colouring
	o_colour = f_colour;
}
)";

static const char *gl_shader_frag_4 = R"(
#version 150 core

in vec2 f_uv;
in vec2 f_cuv;
in vec4 f_colour;

out vec4 o_colour;

uniform usampler2D u_texture;

void main()
{
	// Get index texel
	ivec2 index_texel_coord = ivec2((int(f_uv.x) >> 3u), int(f_uv.y));
	uvec4 index_texel = texelFetch(u_texture, index_texel_coord, 0);
	uint index_subtexel = (index_texel.r >> (((uint(f_uv.x)) & 7u) << 2u)) & 0x0000000Fu;
	
	// Get texel
	ivec2 cuv = ivec2(int(f_cuv.x) + int(index_subtexel), int(f_cuv.y));
	ivec2 texel_coord = ivec2((int(cuv.x) >> 1u), int(cuv.y));
	uvec4 texel = texelFetch(u_texture, texel_coord, 0);
	uint subtexel = (texel.r >> (((uint(cuv.x)) & 1u) << 4u)) & 0x0000FFFFu;
	
	// Get RGBA colour from texel
	float b = float((subtexel & 0x7C00u) >> 10) / 32.0;
	float g = float((subtexel & 0x03E0u) >> 5)  / 32.0;
	float r = float((subtexel & 0x001Fu))       / 32.0;
	
	// Write texel
	if ((subtexel & 0x8000u) != 0u)
	{
		// Enable blending
		o_colour = vec4(r, g, b, 1.0f) * f_colour;
	}
	else
	{
		// No blending
		if ((r + g + b) == 0.0f)
			discard;
		o_colour = vec4(r, g, b, 1.0f) * vec4(f_colour.rgb, 1.0);
	}
}
)";

static const char *gl_shader_frag_8 = R"(
#version 150 core

in vec2 f_uv;
in vec2 f_cuv;
in vec4 f_colour;

out vec4 o_colour;

uniform usampler2D u_texture;

void main()
{
	// Get index texel
	ivec2 index_texel_coord = ivec2((int(f_uv.x) >> 2u), int(f_uv.y));
	uvec4 index_texel = texelFetch(u_texture, index_texel_coord, 0);
	uint index_subtexel = (index_texel.r >> (((uint(f_uv.x)) & 3u) << 3u)) & 0x000000FFu;
	
	// Get texel
	ivec2 cuv = ivec2(int(f_cuv.x) + int(index_subtexel), int(f_cuv.y));
	ivec2 texel_coord = ivec2((int(cuv.x) >> 1u), int(cuv.y));
	uvec4 texel = texelFetch(u_texture, texel_coord, 0);
	uint subtexel = (texel.r >> (((uint(cuv.x)) & 1u) << 4u)) & 0x0000FFFFu;
	
	// Get RGBA colour from texel
	float b = float((subtexel & 0x7C00u) >> 10) / 32.0;
	float g = float((subtexel & 0x03E0u) >> 5)  / 32.0;
	float r = float((subtexel & 0x001Fu))       / 32.0;
	
	// Write texel
	if ((subtexel & 0x8000u) != 0u)
	{
		// Enable blending
		o_colour = vec4(r, g, b, 1.0f) * f_colour;
	}
	else
	{
		// No blending
		if ((r + g + b) == 0.0f)
			discard;
		o_colour = vec4(r, g, b, 1.0f) * vec4(f_colour.rgb, 1.0);
	}
}
)";

static const char *gl_shader_frag_16 = R"(
#version 150 core

in vec2 f_uv;
in vec2 f_cuv;
in vec4 f_colour;

out vec4 o_colour;

uniform usampler2D u_texture;

void main()
{
	// Get texel
	ivec2 texel_coord = ivec2((int(f_uv.x) >> 1u), int(f_uv.y));
	uvec4 texel = texelFetch(u_texture, texel_coord, 0);
	uint subtexel = (texel.r >> (((uint(f_uv.x)) & 1u) << 4u)) & 0x0000FFFFu;
	
	// Get RGBA colour from texel
	float b = float((subtexel & 0x7C00u) >> 10) / 32.0;
	float g = float((subtexel & 0x03E0u) >> 5)  / 32.0;
	float r = float((subtexel & 0x001Fu))       / 32.0;
	
	// Write texel
	if ((subtexel & 0x8000u) != 0u)
	{
		// Enable blending
		o_colour = vec4(r, g, b, 1.0f) * f_colour;
	}
	else
	{
		// No blending
		if ((r + g + b) == 0.0)
			discard;
		o_colour = vec4(r, g, b, 1.0f) * vec4(f_colour.rgb, 1.0);
	}
}
)";

namespace System
{
	namespace GPU
	{
		// GPU GL class
		// Constructor and destructor
		GPU_GL::GPU_GL()
		{
			// Generate VRAM texture
			glGenTextures(1, &vram_texture);
			
			// Set texture parameters
			glBindTexture(GL_TEXTURE_2D, vram_texture);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, VRAM_TEXW / 2, VRAM_TEXH, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
			
			// Clear VRAM
			VRAM_Fill(0x0000, 0, 0, VRAM_TEXW, VRAM_TEXH);
			
			// Compile shaders
			shader_flat.Compile(gl_shader_vert, gl_shader_frag_flat);
			shader_4.Compile(gl_shader_vert, gl_shader_frag_4);
			shader_8.Compile(gl_shader_vert, gl_shader_frag_8);
			shader_16.Compile(gl_shader_vert, gl_shader_frag_16);
		}
		
		GPU_GL::~GPU_GL()
		{
			// Destroy VRAM texture
			glDeleteTextures(1, &vram_texture);
		}
		
		// Screen interface
		void GPU_GL::Screen_Set(unsigned int width, unsigned int height)
		{
			// Use given dimensions
			screen_width = width;
			screen_height = height;
		}
		
		// VRAM interface
		void GPU_GL::VRAM_Fill(uint16_t f, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			// Drop invalid coordinates
			if (w > VRAM_TEXW || x > (VRAM_TEXW - w) || h > VRAM_TEXH || y > (VRAM_TEXH - h))
				return;
			
			// Fill to VRAM
			uint16_t *dst = &vram[y][x];
			for (unsigned int cy = 0; cy < h; cy++)
			{
				std::fill(dst, dst + w, f);
				dst += VRAM_TEXW;
			}
			
			// Upload changes to VRAM texture
			VRAM_Upload(x, y, w, h);
		}
		
		void GPU_GL::VRAM_Write(const uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			// Drop invalid coordinates
			if (w > VRAM_TEXW || x > (VRAM_TEXW - w) || h > VRAM_TEXH || y > (VRAM_TEXH - h))
				return;
			
			// Read from data to VRAM
			uint16_t *dst = &vram[y][x];
			for (unsigned int cy = 0; cy < h; cy++)
			{
				std::copy(data, data + w, dst);
				dst += VRAM_TEXW;
				data += w;
			}
			
			// Upload changes to VRAM texture
			VRAM_Upload(x, y, w, h);
		}
		
		void GPU_GL::VRAM_Read(uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			// Drop invalid coordinates
			if (w > VRAM_TEXW || x > (VRAM_TEXW - w) || h > VRAM_TEXH || y > (VRAM_TEXH - h))
				return;
			
			// Read from VRAM to data
			const uint16_t *src = &vram[y][x];
			for (unsigned int cy = 0; cy < h; cy++)
			{
				std::copy(src, src + w, data);
				src += VRAM_TEXW;
				data += w;
			}
		}
		
		// Render interface
		void GPU_GL::Render_Triangle(const Triangle &tri)
		{
			GLuint batch_vao, batch_vbo;
			
			struct GL_Vertex
			{
				float x, y, z;
				float u, v;
				float cu, cv;
				float r, g, b, a;
			};
			
			static const float tpage_width[3] = {256.0f, 128.0f, 64.0f};
			GL_Shader *shaders[3] = {&shader_4, &shader_8, &shader_16};
			
			// Only modern OpenGL has VAOs
			glGenVertexArrays(1, &batch_vao);
			glBindVertexArray(batch_vao);
			
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			
			// Create batch VBO
			glGenBuffers(1, &batch_vbo);
			
			// Send data
			glBindVertexArray(batch_vao);
			glBindBuffer(GL_ARRAY_BUFFER, batch_vbo);
			
			// Set attribute pointers
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_Vertex), (GLvoid*)(4 * 0));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_Vertex), (GLvoid*)(4 * 3));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_Vertex), (GLvoid*)(4 * 5));
			glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GL_Vertex), (GLvoid*)(4 * 7));
			
			// Send data to VBO
			GL_Vertex verts[3];
			
			float bu = (tri.tpage % VRAM_PAGEW) * tpage_width[tri.bpp];
			float bv = (tri.tpage / VRAM_PAGEW) * 256.0f;
			
			float cu = (tri.cpage % VRAM_CPAGEW) * 16.0f;
			float cv = (tri.cpage / VRAM_CPAGEW);
			
			for (int i = 0; i < 3; i++)
			{
				verts[i].x = tri.vertex[i].x;
				verts[i].y = tri.vertex[i].y;
				verts[i].z = tri.vertex[i].z;
				verts[i].u = bu + tri.vertex[i].u;
				verts[i].v = bv + tri.vertex[i].v;
				verts[i].cu = cu;
				verts[i].cv = cv;
				verts[i].r = (float)tri.vertex[i].r / 128.0f;
				verts[i].g = (float)tri.vertex[i].g / 128.0f;
				verts[i].b = (float)tri.vertex[i].b / 128.0f;
				verts[i].a = 1.0f;
			}
			
			glBufferData(GL_ARRAY_BUFFER, 3 * 4 * 11, (const void*)(&verts[0]), GL_STATIC_DRAW);
			
			// Display data
			glBindTexture(GL_TEXTURE_2D, vram_texture);
			
			if (tri.tpage != TPAGE_NULL)
				glUseProgram(shaders[tri.bpp]->GetProgram());
			else
				glUseProgram(shader_flat.GetProgram());
			
			glDrawArrays(GL_TRIANGLES, 0, 3);
			
			// Delete buffers
			glDeleteBuffers(1, &batch_vbo);
			glDeleteVertexArrays(1, &batch_vao);
		}
		
		// Internal VRAM interface
		void GPU_GL::VRAM_Upload(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			// Copy 32-bit (two pixel) area
			const uint8_t *src = (uint8_t*)(&vram[y][x & ~1]);
			
			w = (w + 1) >> 1;
			x >>= 1;
			
			uint32_t *data = new uint32_t[w * h];
			uint32_t *datap = data;
			
			for (unsigned int cy = 0; cy < h; cy++)
			{
				const uint8_t *srce = src + (VRAM_TEXW << 1);
				for (unsigned int cx = 0; cx < w; cx++)
				{
					*datap++ = (src[3] << 24) | (src[2] << 16) | (src[1] << 8) | (src[0] << 0);
					src += 4;
				}
				src = srce;
			}
			
			// Copy CPU VRAM to texture
			glBindTexture(GL_TEXTURE_2D, vram_texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RED_INTEGER, GL_UNSIGNED_INT, (const void*)data);
			delete[] data;
		}
	}
}
