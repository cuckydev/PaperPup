#pragma once

#include <type_traits>

namespace PaperPup::Types
{

template <typename T, typename OP, OP *OP_PTR, bool SINGLE>
class UniqueGLInstance
{
private:
	using ThisType = UniqueGLInstance<T, OP, OP_PTR, SINGLE>;
	T obj = 0;

	
public:
	// Constructor
	UniqueGLInstance() {}

	// Move
	UniqueGLInstance(ThisType &&other) : obj(other.obj) { other.obj = 0; }
	ThisType &operator=(ThisType &&other)
	{
		obj = other.obj;
		other.obj = 0;
		return *this;
	}

	// Copy
	UniqueGLInstance(const ThisType &) = delete;
	UniqueGLInstance &operator=(const ThisType &) = delete;

	// Destructor
	~UniqueGLInstance()
	{
		Reset();
	}

	// Funcs
	void Reset(T _obj = 0)
	{
		if (obj != 0)
		{
			if constexpr (SINGLE)
				(*OP_PTR)(obj);
			else
				(*OP_PTR)(1, &obj);
		}
		obj = _obj;
	}

	T Release()
	{
		T _obj = obj;
		obj = 0;
		return _obj;
	}

	T Get() const
	{
		return obj;
	}

	T *GetAddressOf()
	{
		return &obj;
	}

	// Operators
	T operator->() const
	{
		return obj;
	}

	typedef T(ThisType:: *bool_)() const;
	operator bool_() const
	{
		if (obj != 0)
			return &ThisType::Get;
		return nullptr;
	}

	bool operator!() const
	{
		return obj == 0;
	}
};

}
