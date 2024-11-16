#pragma once

#include <Leon/Leon.h>

#include "Script/Thread.h"
#include "Script/Context.h"

#include <optional>
#include <type_traits>
#include <string>

namespace PaperPup::Script::Lib
{

namespace detail
{

// Optional SFINAE
template <typename T, typename Enable = void>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

// Is specializations
template <typename T>
bool Is(lua_State *L, int index);

template <>
inline bool Is<std::nullptr_t>(lua_State *L, int index)
{
	return lua_isnil(L, index);
}

template <>
inline bool Is<int>(lua_State *L, int index)
{
	return lua_isnumber(L, index);
}

template <>
inline bool Is<double>(lua_State *L, int index)
{
	return lua_isnumber(L, index);
}

template <>
inline bool Is<bool>(lua_State *L, int index)
{
	return lua_isboolean(L, index);
}

template <>
inline bool Is<std::string>(lua_State *L, int index)
{
	return lua_isstring(L, index);
}

// To specializations
template <typename T>
T To(lua_State *L, int index);

template <>
inline int To<int>(lua_State *L, int index)
{
	return lua_tointeger(L, index);
}

template <>
inline double To<double>(lua_State *L, int index)
{
	return lua_tonumber(L, index);
}

template <>
inline bool To<bool>(lua_State *L, int index)
{
	return lua_toboolean(L, index);
}

template <>
inline std::string To<std::string>(lua_State *L, int index)
{
	size_t length;
	const char *str = lua_tolstring(L, index, &length);
	if (str == nullptr)
		return std::string();
	return std::string(str, length);
}

// Push specializations
template <typename T>
void Push(lua_State *L, const T &value);

template <>
inline void Push<std::nullptr_t>(lua_State *L, const std::nullptr_t &value)
{
	(void)value;
	lua_pushnil(L);
}

template <>
inline void Push<int>(lua_State *L, const int &value)
{
	lua_pushinteger(L, value);
}

template <>
inline void Push<double>(lua_State *L, const double &value)
{
	lua_pushnumber(L, value);
}

template <>
inline void Push<bool>(lua_State *L, const bool &value)
{
	lua_pushboolean(L, value);
}

template <>
inline void Push<std::string>(lua_State *L, const std::string &value)
{
	lua_pushlstring(L, value.c_str(), value.length());
}

}

// Lua type checking interface as in lua_is...
// This can be extended by adding more specializations to detail::Is
template <typename T>
inline bool Is(lua_State *L, int index)
{
	// Check if the type is an optional
	if constexpr (detail::is_optional<T>::value)
	{
		if (lua_isnoneornil(L, index))
			return true;
		else
			return Is<typename T::value_type>(L, index);
	}
	else
	{
		// Check the value
		if (index == 0 || std::abs(index) > lua_gettop(L))
			return false;
		return detail::Is<T>(L, index);
	}
}

// Lua reading interface as in lua_to...
// This can be extended by adding more specializations to detail::Read
template <typename T>
inline T To(lua_State *L, int index)
{
	// Check if the type is an optional
	if constexpr (detail::is_optional<T>::value)
	{
		if (lua_isnoneornil(L, index))
			return std::nullopt;
		else
			return To<typename T::value_type>(L, index);
	}
	else
	{
		// Read the value
		return detail::To<T>(L, index);
	}
}

// Lua writing interface as in lua_push...
// This can be extended by adding more specializations to detail::Write
template <typename T>
inline void Push(lua_State *L, const T &value)
{
	// Check if the type is an optional
	if constexpr (detail::is_optional<T>::value)
	{
		if (value.has_value())
			Push<typename T::value_type>(L, value.value());
		else
			lua_pushnil(L);
	}
	else
	{
		// Write the value
		detail::Push<T>(L, value);
	}
}

// Thread permissions checking
void CheckPermissions(Script::Thread *thread, Context::Permissions permissions);

inline void CheckPermissions(lua_State *thread, Context::Permissions permissions)
{
	CheckPermissions(Script::Thread::GetThread(thread), permissions);
}

}
