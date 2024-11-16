#pragma once

#include "Backend/Render.h"

#include "Types/File.h"
#include "Types/UniqueGLInstance.h"

#include "PS1/TMD.h"
#include "PS1/VDF.h"

#include <memory>

namespace PaperPup
{

class VDFActor : public TMD::Model
{
private:
	std::shared_ptr<VDF::Data> vdf_data;

	Types::UniqueGLInstance<GLuint, decltype(glDeleteBuffers), &glDeleteBuffers, false> vbo_id;

public:
	VDFActor(const std::shared_ptr<TMD::Data> &_tmd_data, const std::shared_ptr<VDF::Data> &_vdf_data);
};

}
