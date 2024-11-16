#pragma once

#include "glad/glad.h"

#include "Types/File.h"
#include "Types/UniqueGLInstance.h"

#include <memory>

namespace PaperPup::PS1
{

class Context
{
public:
	// VRAM texture
	Types::UniqueGLInstance<GLuint, decltype(glDeleteTextures), &glDeleteTextures, false> vram_texture_id;
	
	Context();
	~Context();

	void LoadTIM(const Types::File &tim_file);
};

}
