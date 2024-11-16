#include "Script/Lib.h"

namespace PaperPup::Script::Lib
{

// Thread permissions checking
void CheckPermissions(Script::Thread *thread, Context::Permissions permissions)
{
	// Check permissions
	if (thread == nullptr)
	{
		if (permissions == Context::Permissions::None)
			return;
	}
	if (thread == nullptr || !Context::CanAccess(thread->GetContext().identity, permissions))
		luaL_error(thread->GetState(), "Insufficient permissions ('%s' required, currently '%s')", Context::PermissionsString(permissions), Context::IdentityString(thread->GetContext().identity));
}

}
