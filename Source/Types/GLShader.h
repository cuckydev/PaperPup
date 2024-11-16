#pragma once

#include "glad/glad.h"

#include "Types/UniqueGLInstance.h"
#include "Types/Exceptions.h"

namespace PaperPup::Types
{

struct GLShader
{
	Types::UniqueGLInstance<GLuint, decltype(glDeleteShader), &glDeleteShader, true> vertex;
	Types::UniqueGLInstance<GLuint, decltype(glDeleteShader), &glDeleteShader, true> fragment;
	Types::UniqueGLInstance<GLuint, decltype(glDeleteProgram), &glDeleteProgram, true> program;

	GLShader(const char *vertex_src, const char *fragment_src)
	{
		static GLchar info_log[512];
		GLint success;

		// Create objects
		vertex.Reset(glCreateShader(GL_VERTEX_SHADER));
		fragment.Reset(glCreateShader(GL_FRAGMENT_SHADER));
		program.Reset(glCreateProgram());

		// Compile vertex
		glShaderSource(vertex.Get(), 1, &vertex_src, nullptr);
		glCompileShader(vertex.Get());

		// Check for errors
		glGetShaderiv(vertex.Get(), GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex.Get(), sizeof(info_log) / sizeof(GLchar), nullptr, info_log);
			throw Types::RuntimeException(info_log);
		}

		// Compile fragment
		glShaderSource(fragment.Get(), 1, &fragment_src, nullptr);
		glCompileShader(fragment.Get());

		// Check for errors
		glGetShaderiv(fragment.Get(), GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment.Get(), sizeof(info_log) / sizeof(GLchar), nullptr, info_log);
			throw Types::RuntimeException(info_log);
		}

		// Link
		glAttachShader(program.Get(), vertex.Get());
		glAttachShader(program.Get(), fragment.Get());
		glLinkProgram(program.Get());

		glDetachShader(program.Get(), vertex.Get());
		glDetachShader(program.Get(), fragment.Get());

		// Check for errors
		glGetProgramiv(program.Get(), GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(program.Get(), sizeof(info_log) / sizeof(GLchar), nullptr, info_log);
			throw Types::RuntimeException(info_log);
		}
	}

	void Bind() const
	{
		glUseProgram(program.Get());
	}
};

}
