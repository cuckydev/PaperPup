#pragma once

#include "Script/Context.h"

#include "Types/File.h"

#include "lua.h"
#include "lualib.h"

#include <string>
#include <memory>

namespace PaperPup::Script
{

namespace detail
{

// Lua singleton state
typedef std::unique_ptr<lua_State, decltype(&lua_close)> SingletonState;
lua_State *Singleton();

lua_State *NewThread();

}

class Thread;

class ThreadRef
{
private:
	lua_State *thread_state = nullptr;
	int thread_ref = LUA_NOREF;

	friend class Thread;

	ThreadRef(lua_State *state, Thread &thread);

	void Register(Thread &thread);
	void Release();

public:
	~ThreadRef();

	lua_State *GetState()
	{
		return thread_state;
	}
};

class Thread
{
private:
	std::shared_ptr<ThreadRef> ref;

	Context context;

public:
	Thread();
	Thread(const std::string &source);

	Thread(Types::File &file) : Thread(file.GetString()) {}

	~Thread();

	lua_State *GetState()
	{
		return ref->GetState();
	}

	Thread(const Thread &other) = delete;
	Thread &operator=(const Thread &other) = delete;

	Thread(Thread &&other) noexcept
	{
		*this = std::move(other);
	}

	Thread &operator=(Thread &&other) noexcept
	{
		if (ref != nullptr)
			ref->Release();

		ref = std::move(other.ref);
		ref->Register(*this);

		context = std::move(other.context);
		other.context = Context();

		return *this;
	}

	void SetContext(const Context &new_context)
	{
		context = new_context;
	}
	const Context &GetContext() const
	{
		return context;
	}

	void Resume();

	static Thread *GetThread(lua_State *L);
};

}
