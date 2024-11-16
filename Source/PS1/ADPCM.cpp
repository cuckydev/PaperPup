#include "PS1/ADPCM.h"

#include "Types/Exceptions.h"

namespace PaperPup::PS1
{

static const struct
{
	char f0, f1;
} FILTERS[8] = {
	{   0,   0 },
	{  60,   0 },
	{ 115, -52 },
	{  98, -55 },
	{ 122, -60 }, // Filter 4 is only available to SPU ADPCM
	{   0,   0 },
	{   0,   0 },
	{   0,   0 }
};

int16_t ADPCMDecode::DecodeNibble(unsigned char nibble, unsigned char shift, char f0, char f1)
{
	// Sign extend nibble
	int32_t t = nibble;
	if (t & 0x8)
		t -= 0x10;

	// Decode and clip sample
	int32_t s = ((t << 12) >> shift) + ((f0 * filter_old + f1 * filter_older + 32) / 64);
	if (s < -0x7FFF)
		s = -0x7FFF;
	if (s > 0x7FFF)
		s = 0x7FFF;

	// Update filter
	filter_older = filter_old;
	filter_old = s;

	return static_cast<int16_t>(s);
}

int16_t ADPCMDecode::DecodeByte(unsigned char byte, unsigned char shift, char f0, char f1)
{
	// Sign extend byte
	int32_t t = byte;
	if (t & 0x80)
		t -= 0x100;

	// Decode and clip sample
	int32_t s = ((t << 8) >> shift) + ((f0 * filter_old + f1 * filter_older + 32) / 64);
	if (s < -0x7FFF)
		s = -0x7FFF;
	if (s > 0x7FFF)
		s = 0x7FFF;

	// Update filter
	filter_older = filter_old;
	filter_old = s;

	return static_cast<int16_t>(s);
}

void ADPCMDecode::DecodeSPUBlock(Types::Span<const char, 16> block, Types::Span<int16_t, 28> out)
{
	unsigned char shift_filter = static_cast<unsigned char>(block[0]);
	// unsigned char flags = static_cast<unsigned char>(block[1]);

	unsigned char shift = shift_filter & 0x0F;
	if (shift > 12)
		shift = 9;
	unsigned char filter = (shift_filter >> 4) & 0x07;

	char f0 = FILTERS[filter].f0;
	char f1 = FILTERS[filter].f1;

	// Decode nibbles
	int16_t *out_p = out.Data();

	for (size_t i = 2; i < 16; i++)
	{
		*out_p++ = DecodeNibble((static_cast<unsigned char>(block[i]) >> 0) & 0xF, shift, f0, f1);
		*out_p++ = DecodeNibble((static_cast<unsigned char>(block[i]) >> 4) & 0xF, shift, f0, f1);
	}
}

template <bool FmtStereo, bool Fmt18900Hz, bool Fmt8Bit>
void XADecode::DecodeXAData(Types::Span<const char, 0x920> sector, XASectorOut &out)
{
	// Setup output
	out.sample_rate = Fmt18900Hz ? 18900 : 37800;

	out.samples_count = 8 * 28 * 0x12;
	if constexpr (Fmt8Bit) out.samples_count /= 2;
	if constexpr (FmtStereo) out.samples_count /= 2;

	int16_t *out_p = *out.samples.data();

	// Decode samples
	size_t sector_p = 8;
	for (size_t i = 0; i < 0x12; i++)
	{
		auto header = sector.Slice<8>(sector_p + 4);

		// The header is actually 8 bytes, but it's padded on both sides with the same data
		// So the outer 4 bytes must match the inner 4 bytes
		if (std::memcmp(header.Data(), header.Data() - 4, 4) || std::memcmp(header.Data() + 4, header.Data() + 8, 4))
		{
			// The whole sector is presumed to be bad
			out.samples_count = 0;
			return;
		}

		for (size_t b = 0; b < (Fmt8Bit ? 4 : 8); b += (FmtStereo ? 2 : 1))
		{
			// Read headers
			unsigned char header_l, header_r;

			unsigned char shift_l, shift_r;
			char f0_l, f1_l, f0_r, f1_r;

			if constexpr (FmtStereo)
			{
				header_l = static_cast<unsigned char>(header[b + 0]);
				header_r = static_cast<unsigned char>(header[b + 1]);
			}
			else
			{
				header_l = static_cast<unsigned char>(header[b]);

				(void)header_r;
			}

			shift_l = header_l & 0x0F;
			if (shift_l > 12)
				shift_l = 9;
			unsigned char filter_l = (header_l >> 4) & 0x03;
			f0_l = FILTERS[filter_l].f0;
			f1_l = FILTERS[filter_l].f1;

			if constexpr (FmtStereo)
			{
				shift_r = header_r & 0x0F;
				if (shift_r > 12)
					shift_r = 9;
				unsigned char filter_r = (header_r >> 4) & 0x03;
				f0_r = FILTERS[filter_r].f0;
				f1_r = FILTERS[filter_r].f1;
			}
			else
			{
				(void)shift_r;
				(void)f0_r;
				(void)f1_r;
			}

			// Decode sample words
			auto *word_p = &sector[sector_p + 16];

			for (size_t w = 0; w < 28; w++)
			{
				uint32_t word = 0;
				word |= (static_cast<uint32_t>(word_p[0]) & 0xFF) << 0;
				word |= (static_cast<uint32_t>(word_p[1]) & 0xFF) << 8;
				word |= (static_cast<uint32_t>(word_p[2]) & 0xFF) << 16;
				word |= (static_cast<uint32_t>(word_p[3]) & 0xFF) << 24;
				word_p += 4;

				int16_t sample_l, sample_r;
				if constexpr (Fmt8Bit)
				{
					sample_l = adpcm_l.DecodeByte((word >> ((b + 0) * 8)) & 0xFF, shift_l, f0_l, f1_l);
					if constexpr (FmtStereo)
						sample_r = adpcm_r.DecodeByte((word >> ((b + 1) * 8)) & 0xFF, shift_r, f0_r, f1_r);
					else
						sample_r = sample_l;
				}
				else
				{
					sample_l = adpcm_l.DecodeNibble((word >> ((b + 0) * 4)) & 0xF, shift_l, f0_l, f1_l);
					if constexpr (FmtStereo)
						sample_r = adpcm_r.DecodeNibble((word >> ((b + 1) * 4)) & 0xF, shift_r, f0_r, f1_r);
					else
						sample_r = sample_l;
				}

				*out_p++ = sample_l;
				*out_p++ = sample_r;
			}
		}

		sector_p += 0x80;
	}
}

void XADecode::DecodeXASector(Types::Span<const char, 0x920> sector, XASectorOut &out)
{
	// The subheader is repeated twice in the sector
	auto subheader = reinterpret_cast<const SubHeader *>(sector.Data());
	if (std::memcmp(&subheader[0], &subheader[1], sizeof(SubHeader)))
	{
		// The whole sector is presumed to be bad
		out.samples_count = 0;
		return;
	}

	// Determine XA format
	bool fmt_stereo = ((subheader->codinginfo >> 0) & 0x03) == 0x01;
	bool fmt_18900hz = ((subheader->codinginfo >> 2) & 0x03) == 0x01;
	bool fmt_8bit = ((subheader->codinginfo >> 4) & 0x03) == 0x01;
	
	if (fmt_stereo)
	{
		if (fmt_18900hz)
		{
			if (fmt_8bit)
			{
				DecodeXAData<true, true, true>(sector, out);
			}
			else
			{
				DecodeXAData<true, true, false>(sector, out);
			}
		}
		else
		{
			if (fmt_8bit)
			{
				DecodeXAData<true, false, true>(sector, out);
			}
			else
			{
				DecodeXAData<true, false, false>(sector, out);
			}
		}
	}
	else
	{
		if (fmt_18900hz)
		{
			if (fmt_8bit)
			{
				DecodeXAData<false, true, true>(sector, out);
			}
			else
			{
				DecodeXAData<false, true, false>(sector, out);
			}
		}
		else
		{
			if (fmt_8bit)
			{
				DecodeXAData<false, false, true>(sector, out);
			}
			else
			{
				DecodeXAData<false, false, false>(sector, out);
			}
		}
	}
}

}
