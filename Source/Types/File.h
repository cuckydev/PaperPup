#pragma once

#include "Types/Span.h"

#include "Types/Exceptions.h"

namespace PaperPup::Types
{

class File
{
protected:
	Span<char> span;

	File(Span<char> _span) : span(_span) {}

public:
	virtual ~File() {}

	Span<char> GetSpan(size_t align = 0) const
	{
		if (align != 0 && reinterpret_cast<uintptr_t>(span.Data()) % align != 0)
			throw RuntimeException("File alignment requirement not met");
		return span;
	}

	template <typename T>
	Span<T> GetSubspan(size_t offset, size_t size) const
	{
		if (offset + size * sizeof(T) > span.Size())
			throw RuntimeException("Data offset out of bounds");
		if (alignof(T) != 0 && reinterpret_cast<uintptr_t>(span.Data() + offset) % alignof(T) != 0)
			throw RuntimeException("Data alignment requirement not met");
		return Span<T>(reinterpret_cast<T*>(span.Data() + offset), size);
	}

	template <typename T>
	T &Get(size_t offset) const
	{
		if (offset + sizeof(T) > span.Size())
			throw RuntimeException("Data offset out of bounds");
		if (alignof(T) != 0 && reinterpret_cast<uintptr_t>(span.Data() + offset) % alignof(T) != 0)
			throw RuntimeException("Data alignment requirement not met");
		return *reinterpret_cast<T*>(span.Data() + offset);
	}

	std::string GetString() const
	{
		return std::string(span.begin(), span.end());
	}
};

}
