/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <cstdint>

namespace System
{
	namespace SPU
	{
		namespace ADPCM
		{
			//ADPCM decoder class
			class Decoder
			{
				private:
					//Decode state
					int16_t old = 0, older = 0;
					
				public:
					//Decode interface
					void Reset() { old = 0; older = 0; }
					
					void DecodeBlock(uint8_t header, const uint8_t *block, int16_t *decode); //SPU ADPCM
					void DecodeSector4(uint8_t header, const uint8_t *block, int block_i, int16_t *decode); //XA ADPCM 4-bit
					void DecodeSector8(uint8_t header, const uint8_t *block, int block_i, int16_t *decode); //XA ADPCM 8-bit
			};
		}
	}
}
