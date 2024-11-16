#pragma once

namespace PaperPup::Types
{

template <typename T, void (*OP)(T), T NUL = nullptr>
class UniqueCInstance
{
private:
	using ThisType = UniqueCInstance<T, OP, NUL>;
	T obj = NUL;

public:
	// Constructor
	UniqueCInstance() {}

	UniqueCInstance(T _obj) : obj(_obj) {}
	UniqueCInstance(std::nullptr_t) {}

	// Move
	UniqueCInstance(ThisType &&other) : obj(other.obj) { other.obj = NUL; }
	ThisType &operator=(ThisType &&other)
	{
		obj = other.obj;
		other.obj = NUL;
		return *this;
	}
	ThisType &operator=(std::nullptr_t)
	{
		Reset();
		return *this;
	}

	// Copy
	UniqueCInstance(const ThisType &) = delete;
	UniqueCInstance &operator=(const ThisType &) = delete;

	// Destructor
	~UniqueCInstance()
	{
		Reset();
	}

	// Funcs
	void Reset(T _obj = NUL)
	{
		if (obj != NUL)
			OP(obj);
		obj = _obj;
	}

	T Release()
	{
		T _obj = obj;
		obj = NUL;
		return _obj;
	}

	T Get() const
	{
		return obj;
	}

	T *GetAddressOf() const
	{
		return obj;
	}

	// Operators
	T operator->() const
	{
		return obj;
	}

	typedef T(ThisType:: *bool_)() const;
	operator bool_() const
	{
		if (obj != NUL)
			return &ThisType::Get;
		return nullptr;
	}

	bool operator!() const
	{
		return obj == NUL;
	}
};

}
