#include "PS1/VDFActor.h"

namespace PaperPup
{

VDFActor::VDFActor(const std::shared_ptr<TMD::Data> &_tmd_data, const std::shared_ptr<VDF::Data> &_vdf_data) : TMD::Model(_tmd_data), vdf_data(_vdf_data)
{

}

}
