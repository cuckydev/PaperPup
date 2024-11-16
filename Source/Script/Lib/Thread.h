#pragma once

#include "Script/Lib.h"

namespace PaperPup::Script::Lib::Thread
{

void LEON LEON_V("Yield") LEON_KV("Permissions", "Script") Wait(lua_State *L, const std::optional<lua_Number> &seconds);

}
