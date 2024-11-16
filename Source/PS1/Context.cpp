#include "Context.h"

#include "PS1/TIM.h"

#include "Util/Endian.h"

#include "Types/Exceptions.h"

#include <iostream>

namespace PaperPup::PS1
{

Context::Context()
{
	// Create 1024x512 VRAM texture (R16U)
	glGenTextures(1, vram_texture_id.GetAddressOf());
	glBindTexture(GL_TEXTURE_2D, vram_texture_id.Get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, 1024, 512, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Context::~Context()
{

}

void Context::LoadTIM(const Types::File &tim_file)
{
	glBindTexture(GL_TEXTURE_2D, vram_texture_id.Get());
	glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

	const auto &tim_header = tim_file.Get<TIM::Header>(0);

	if (Endian::SwapLE(tim_header.id) != 0x10)
		throw Types::RuntimeException("Invalid TIM file");

	const auto type = Endian::SwapLE(tim_header.type);
	switch (type & TIM::Header::Type_BPP_Mask)
	{
		case TIM::Header::Type_16Bit:
		case TIM::Header::Type_4Bit:
		case TIM::Header::Type_8Bit:
		case TIM::Header::Type_24Bit:
		case TIM::Header::Type_Mixed:
			break;
		default:
			throw Types::RuntimeException("Unsupported TIM type");
	}

	size_t tim_p = sizeof(TIM::Header);

	bool is_clut = (type & TIM::Header::Type_HasCLUT) != 0;
	for (int i = is_clut; i-- >= 0;)
	{
		const auto &image_header = tim_file.Get<TIM::ImageHeader>(tim_p);

		const auto size = Endian::SwapLE(image_header.size);

		const auto next_p = tim_p + size;
		tim_p += sizeof(TIM::ImageHeader);

		const auto x = Endian::SwapLE(image_header.x);
		const auto y = Endian::SwapLE(image_header.y);
		const auto w = Endian::SwapLE(image_header.w);
		const auto h = Endian::SwapLE(image_header.h);

		if (x + w > 1024 || y + h > 512)
			throw Types::RuntimeException("TIM image out of bounds");

		size_t data_size = 2ULL * w * h;
		if (data_size > (size + sizeof(TIM::ImageHeader)))
			throw Types::RuntimeException("TIM image data out of bounds");

		const auto data_span = tim_file.GetSubspan<uint16_t>(tim_p, data_size / 2);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data_span.Data());

		tim_p = next_p;
	}
}

}
