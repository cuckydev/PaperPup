#pragma once

#include <cstdint>

namespace PaperPup::TIM
{

// TIM file structures
struct Header
{
	enum Type
	{
		Type_4Bit = 0x00,
		Type_8Bit = 0x01,
		Type_16Bit = 0x02,
		Type_24Bit = 0x03, // Not expected
		Type_Mixed = 0x04, // Not expected

		Type_BPP_Mask = 0x07,

		Type_HasCLUT = 0x08,
	};

	uint32_t id; // = 0x10
	uint32_t type;
};
static_assert(sizeof(Header) == 8);
static_assert(alignof(Header) <= 4);

// Shared between palette and texture
struct ImageHeader
{
	uint32_t size;
	uint16_t x, y, w, h;
};
static_assert(sizeof(ImageHeader) == 12);
static_assert(alignof(ImageHeader) <= 4);

}
