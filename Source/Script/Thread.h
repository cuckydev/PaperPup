#pragma once

#include "Script/Context.h"

#include "Log/Assert.h"
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
// This loads all the standard libraries and acts as the main Lua state
typedef std::unique_ptr<lua_State, decltype(&lua_close)> SingletonState;
lua_State &Singleton();

// Creates a new sandboxed Lua thread off the singleton state
lua_State &NewThread();

}

class Thread;

class ThreadRef
{
private:
	lua_State *thread_state = nullptr;
	int thread_ref = LUA_NOREF;

	friend class Thread;

	ThreadRef(lua_State &state, Thread &thread);

	void Register(Thread &thread);
	void Release();

public:
	~ThreadRef();

	lua_State &GetState()
	{
		return *thread_state;
	}
};

// Manages a Lua thread, providing a C++ interface and ensuring the security context is maintained
// A lua state should not be usable without going through this class.
class Thread
{
private:
	std::shared_ptr<ThreadRef> ref;

	Context context;

public:
	Thread();
	Thread(const std::string &name, const std::string &source);

	Thread(const std::string &name, Types::File &file) : Thread(name, file.GetString()) {}

	~Thread();

	bool IsDead() const
	{
		return ref == nullptr;
	}

	const std::shared_ptr<ThreadRef> &GetRef()
	{
		return ref;
	}

	lua_State &GetState()
	{
		Log::Assert(!IsDead(), "Cannot use dead thread");
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

	static Thread *GetThread(lua_State &L);
};

}
