#pragma once

#include <filesystem>
#include <fstream>

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

class StlFile : public File
{
private:
	// This is a bit evil
	// Since GetSpan is under the File initialization, it's done before `data` is initialized
	// So we leave it uninitialized (so it isn't wiped later) and also cannot use a unique_ptr
	char *data;

	Types::Span<char> GetSpan(std::ifstream &stream)
	{
		data = nullptr;

		stream.seekg(0, std::ios::end);
		size_t size = static_cast<size_t>(stream.tellg());
		stream.seekg(0, std::ios::beg);

		std::unique_ptr<char[]> data_tmp = std::make_unique<char[]>(size);

		stream.read(data_tmp.get(), static_cast<std::streamsize>(size));

		data = data_tmp.release();

		return Types::Span<char>(data, size);
	}

	Types::Span<char> GetSpan(const std::filesystem::path &path)
	{
		data = nullptr;

		std::ifstream stream(path, std::ios::binary);
		if (!stream)
			throw RuntimeException("Failed to open file");
		return GetSpan(stream);
	}

public:
	StlFile(std::ifstream &stream) : File(GetSpan(stream)) {}
	StlFile(const std::filesystem::path &path) : File(GetSpan(path)) {}

	~StlFile()
	{
		delete[] data;
	}
};

}
