#pragma once

#include <limits>
#include <type_traits>
#include <array>
#include <iterator>
#include <stdexcept>

namespace PaperPup::Types
{

namespace detail
{
inline constexpr size_t dynamic_extent = std::numeric_limits<std::size_t>::max();

template <typename T, bool DynamicExtent>
struct SpanData;

template <typename T>
struct SpanData<T, true>
{
	T *ptr = nullptr;
	size_t size = 0;
};

template <typename T>
struct SpanData<T, false>
{
	T *ptr = nullptr;
};

}

// Non-owning view of a contiguous sequence of objects
template <typename T, size_t Extent = detail::dynamic_extent>
class Span
{
private:
	detail::SpanData<T, Extent == detail::dynamic_extent> data;

public:
	constexpr Span(const Span &other) = default;
	constexpr Span(Span &&other) = default;

	constexpr Span() = default;

	explicit(Extent != detail::dynamic_extent)
	constexpr Span(T *ptr) : data{ptr} {}

	// explicit(Extent != detail::dynamic_extent)
	// constexpr Span(T (&arr)[Extent]) : data{arr} {}

	explicit(Extent != detail::dynamic_extent)
	constexpr Span(std::array<T, Extent> &arr) : data{ arr.data() } {}

	// Construct from dynamic extent
	explicit(Extent == detail::dynamic_extent)
	constexpr Span(T *ptr, size_t size) : data{ptr, size} {}

	// Assignments
	constexpr Span &operator=(const Span &other) = default;
	constexpr Span &operator=(Span &&other) = default;

	// Access
	constexpr T &operator[](size_t idx) const
	{
		return data.ptr[idx];
	}

	constexpr T *Data() const
	{
		return data.ptr;
	}

	constexpr size_t Size() const
	{
		if constexpr (Extent == detail::dynamic_extent)
			return data.size;
		else
			return Extent;
	}
	
	// Slice
	template <size_t SubExtent>
	auto Slice(size_t offset) const
	{
		return Span<T, SubExtent>(data.ptr + offset);
	}

	auto Slice(size_t offset) const
	{
		return Span<T>(data.ptr + offset, data.size - offset);
	}

	auto Slice(size_t offset, size_t count) const
	{
		return Span<T>(data.ptr + offset, count);
	}

	template <size_t SubExtent>
	auto CheckedSlice(size_t offset) const
	{
		if (offset > Size() || Size() - offset < SubExtent)
			throw std::out_of_range("Invalid slice");
		return Slice<SubExtent>(offset);
	}

	auto CheckedSlice(size_t offset) const
	{
		if (offset > Size())
			throw std::out_of_range("Invalid slice");
		return Slice(offset);
	}

	auto CheckedSlice(size_t offset, size_t count) const
	{
		if (offset > Size() || Size() - offset < count)
			throw std::out_of_range("Invalid slice");
		return Slice(offset, count);
	}

	// Iterators
	constexpr T *begin() const
	{
		return data.ptr;
	}

	constexpr T *end() const
	{
		return data.ptr + Size();
	}
};

template <typename T, size_t Extent>
Span(T(&arr)[Extent]) -> Span<T, Extent>;

template <typename T, size_t Extent>
Span(std::array<T, Extent> &arr) -> Span<T, Extent>;

template <typename T>
Span(T *ptr, size_t size) -> Span<T>;

}
