/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "GPU.h"

#include "glad/glad.h"

#include "Main.h"

namespace System
{
	namespace GPU
	{
		//GL shader class
		class GL_Shader
		{
			private:
				//Shader objects
				GLuint program = 0, vertex = 0, fragment = 0;
				
			public:
				//Destructor
				~GL_Shader()
				{
					//Destroy shader objects
					glDeleteProgram(program);
					glDeleteShader(vertex);
					glDeleteShader(fragment);
				}
				
				//Shader interface
				void Compile(const char *src_vert, const char *src_frag)
				{
					//Create shader
					GLint shader_status;
					program = glCreateProgram();
					
					//Compile vertex shader
					vertex = glCreateShader(GL_VERTEX_SHADER);
					glShaderSource(vertex, 1, &src_vert, nullptr);
					glCompileShader(vertex);
					
					glGetShaderiv(vertex, GL_COMPILE_STATUS, &shader_status);
					if (shader_status != GL_TRUE)
					{
						char buffer[0x200];
						glGetShaderInfoLog(vertex, sizeof(buffer), nullptr, buffer);
						throw PaperPup::Exception("[System::GPU::GL_Shader::Compile] Failed to compile vertex shader:\n" + std::string(buffer));
					}
					
					//Compile fragment shader
					fragment = glCreateShader(GL_FRAGMENT_SHADER);
					glShaderSource(fragment, 1, &src_frag, nullptr);
					glCompileShader(fragment);
					
					glGetShaderiv(fragment, GL_COMPILE_STATUS, &shader_status);
					if (shader_status != GL_TRUE)
					{
						char buffer[0x200];
						glGetShaderInfoLog(fragment, sizeof(buffer), nullptr, buffer);
						throw PaperPup::Exception("[System::GPU::GL_Shader::Compile] Failed to compile fragment shader:\n" + std::string(buffer));
					}
					
					//Attach and link
					glAttachShader(program, vertex);
					glAttachShader(program, fragment);
					
					glBindAttribLocation(program, 0, "v_position");
					glBindAttribLocation(program, 1, "v_uv");
					glBindAttribLocation(program, 2, "v_colour");
					
					glLinkProgram(program);
					
					glGetProgramiv(program, GL_LINK_STATUS, &shader_status);
					if (shader_status != GL_TRUE)
					{
						char buffer[0x200];
						glGetProgramInfoLog(program, sizeof(buffer), nullptr, buffer);
						throw PaperPup::Exception("[System::GPU::GL_Shader::Compile] Failed to link shader program:\n" + std::string(buffer));
					}
					
					glDetachShader(program, vertex);
					glDetachShader(program, fragment);
				}
				
				//Gets and sets
				GLuint GetProgram() { return program; }
		};
		
		//GPU GL class
		class GPU_GL : public GPU
		{
			private:
				//VRAM
				uint16_t vram[VRAM_TEXH][VRAM_TEXW]{};
				GLuint vram_texture;
				
				//Shaders
				GL_Shader shader_flat, shader_4, shader_8, shader_16;
				
			public:
				//Constructor and destructor
				GPU_GL();
				~GPU_GL() override;
				
				//Screen interface
				void Screen_Set(unsigned int width, unsigned int height) override;
				
				//VRAM interface
				void VRAM_Fill(uint16_t f, unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;
				void VRAM_Write(const uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;
				void VRAM_Read(uint16_t *data, unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;
				
			private:
				//Internal VRAM interface
				void VRAM_Upload(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
		};
	}
}
