#pragma once

#include "Types/GLShader.h"

namespace PaperPup::PS1
{

class Shader
{
public:
	Types::GLShader shader;

private:
	Shader();
	~Shader();

public:
	static Shader &Instance();
};

}
