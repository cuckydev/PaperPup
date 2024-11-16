#include "PS1/Shader.h"

static const char *vertex_shader =
#include "GLSL/Vertex.h"
;

static const char *fragment_shader =
#include "GLSL/Fragment.h"
;


namespace PaperPup::PS1
{

Shader::Shader() : shader(vertex_shader, fragment_shader)
{

}
Shader::~Shader()
{

}

Shader &Shader::Instance()
{
	static Shader instance;
	return instance;
}

}
