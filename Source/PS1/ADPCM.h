#pragma once

#include <array>
#include <cstdint>

#include "Types/Span.h"

namespace PaperPup::PS1
{

struct SubHeader
{
	uint8_t file, channel;
	uint8_t submode, codinginfo;
};

class ADPCMDecode
{
private:
	int32_t filter_old = 0, filter_older = 0;

public:
	void Reset()
	{
		filter_old = 0;
		filter_older = 0;
	}

	int16_t DecodeNibble(unsigned char nibble, unsigned char shift, char f0, char f1);
	int16_t DecodeByte(unsigned char byte, unsigned char shift, char f0, char f1);

	void DecodeSPUBlock(Types::Span<const char, 16> block, Types::Span<int16_t, 28> out);
};

class XADecode
{
public:
	struct XASectorOut
	{
		uint32_t samples_count = 0;
		uint32_t sample_rate = 0;
		std::array<int16_t[2], 28 * 8 * 0x12> samples;
	};

private:
	ADPCMDecode adpcm_l, adpcm_r;

	template <bool FmtStereo, bool Fmt18900Hz, bool Fmt8Bit>
	void DecodeXAData(Types::Span<const char, 0x920> sector, XASectorOut &out);

public:
	void Reset()
	{
		adpcm_l.Reset();
		adpcm_r.Reset();
	}

	void DecodeXASector(Types::Span<const char, 0x920> sector, XASectorOut &out);
};

}
