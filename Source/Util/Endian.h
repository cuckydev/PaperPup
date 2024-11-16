#pragma once

#include <bit>
#include <type_traits>
#include <array>
#include <algorithm>

namespace PaperPup::Endian
{

template<typename T>
inline constexpr T Swap(T value) noexcept
{
	static_assert(std::has_unique_object_representations_v<T>, "T may not have padding bits");
	auto value_representation = std::bit_cast<std::array<std::byte, sizeof(T)>>(value);
	std::ranges::reverse(value_representation);
	return std::bit_cast<T>(value_representation);
}

template<typename T>
inline constexpr T SwapLE(T value) noexcept
{
	if constexpr (std::endian::native != std::endian::little)
		return Swap(value);
	else
		return value;
}

template<typename T>
inline constexpr T SwapBE(T value) noexcept
{
	if constexpr (std::endian::native != std::endian::big)
		return Swap(value);
	else
		return value;
}

}
