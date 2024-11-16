#pragma once

#include "lua.h"
#include "lualib.h"

#include "Script/Lib.h"

#include <type_traits>

namespace PaperPup::Script
{

class TableContext;

class Table
{
private:
	friend class TableContext;

	TableContext &ctx;
	lua_State &state;
	int idx;

	Table(TableContext &_ctx, lua_State &L, int _idx) : ctx(_ctx), state(L), idx(_idx)
	{
		if (idx < 0)
			idx = lua_gettop(&state) + 1 + idx;
	}

public:
	template <typename T>
	T Get(const char *name)
	{
		if constexpr (std::is_same_v<T, Table>)
		{
			lua_getfield(&state, idx, name);

			if (!lua_istable(&state, -1))
				throw Types::RuntimeException("Table::Get: Value is not of expected type");

			return Table(ctx, state, -1);
		}
		else
		{
			lua_getfield(&state, idx, name);

			if (!Lib::Is<T>(state, -1))
				throw Types::RuntimeException("Table::Get: Value is not of expected type");

			T value = Lib::To<T>(state, -1);
			lua_pop(&state, 1);
			return value;
		}
	}
};

// Table context of a lua_State
// Manages the stack for concurrent table access
class TableContext
{
private:
	lua_State &state;

public:
	TableContext(lua_State &L) : state(L)
	{

	}

	~TableContext()
	{

	}

	Table GetTable(int index)
	{
		if (!lua_istable(&state, index))
			throw Types::RuntimeException("TableContext::GetTable: Index is not a table");
		return Table(*this, state, index);
	}
};

}
