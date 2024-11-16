#pragma once

namespace PaperPup::Script
{

struct Context
{
	// Context enums
	// Each thread context is assigned an identity
	// Each library function is assigned a permissions level
	// Permissions are checked against the identity of the calling thread

	enum class Identity
	{
		Anonymous, // Not assigned an identity, can't access anything

		IndexScript, // A mod index script, limited library access

		UserScript, // A script provided via mod data, can access most library functions

		CoreScript, // A script provided by the core data folder, full library access
	};

	static inline const char *IdentityString(Identity identity)
	{
		switch (identity)
		{
			case Identity::Anonymous:
				return "Anonymous";
			case Identity::IndexScript:
				return "IndexScript";
			case Identity::UserScript:
				return "UserScript";
			case Identity::CoreScript:
				return "CoreScript";
			default:
				return "Unknown";
		}
	}

	enum class Permissions
	{
		None, // Default, anyone can access

		Script, // Only scripts can access

		CoreScript, // Only core scripts can access
	};

	static inline const char *PermissionsString(Permissions permissions)
	{
		switch (permissions)
		{
			case Permissions::None:
				return "None";
			case Permissions::Script:
				return "Script";
			case Permissions::CoreScript:
				return "CoreScript";
			default:
				return "Unknown";
		}
	}

	// Access checks
	static inline bool CanAccess(Identity identity, Permissions permissions)
	{
		if (permissions == Permissions::None)
			return true;
		
		switch (identity)
		{
			case Identity::Anonymous:
				return false;
			case Identity::IndexScript:
				return false;
			case Identity::UserScript:
				return permissions == Permissions::Script;
			case Identity::CoreScript:
				return permissions == Permissions::CoreScript;
			default:
				return false;
		}
	}

	inline bool CanAccess(Permissions permissions) const
	{
		return CanAccess(identity, permissions);
	}

	// Context data
	Identity identity = Identity::Anonymous;
};

}
