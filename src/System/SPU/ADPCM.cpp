/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ADPCM.h"

namespace System
{
	namespace SPU
	{
		namespace ADPCM
		{
			//ADPCM filters
			static const int filter_pos[] = {   0,  60, 115,  98, 122};
			static const int filter_neg[] = {   0,   0, -52, -55, -60};
			
			//ADPCM decoder class
			//Decode interface
			void Decoder::DecodeBlock(uint8_t header, const uint8_t (&block)[14], int16_t (&decode)[28]) //SPU ADPCM
			{
				//Read header
				int shift = 12 - (header & 0x0F);
				int filter = (header & 0x70) >> 4;
				
				int f0 = filter_pos[filter];
				int f1 = filter_neg[filter];
				
				//Decode nibbles
				for (int i = 0; i < 28; i++)
				{
					//Decode sample
					int t = (block[i >> 1] >> ((i & 1) << 2)) & 0xF;
					
					int s = (t << shift) + ((old * f0 + older * f1 + 32) / 64);
					if (s < -0x7FFF)
						s = -0x7FFF;
					if (s > 0x7FFF)
						s = 0x7FFF;
					
					//Write sample
					decode[i] = s;
					older = old;
					old = s;
				}
			}
			
			void Decoder::DecodeSector4(uint8_t header, const uint8_t (&block)[112], int block_i, int16_t (&decode)[28]) //XA ADPCM 4-bit
			{
				//Read header
				int shift = 12 - (header & 0x0F);
				int filter = (header & 0x30) >> 4;
				
				int f0 = filter_pos[filter];
				int f1 = filter_neg[filter];
				
				//Decode nibbles
				const uint8_t *blockp = block + ((block_i >> 1) ^ 0x3);
				int t_shift = (block_i & 1) << 2;
				
				for (int i = 0; i < 28; i++)
				{
					//Decode sample
					int t = (*blockp >> t_shift) & 0xF;
					blockp += 4;
					
					int s = (t << shift) + ((old * f0 + older * f1 + 32) / 64);
					if (s < -0x7FFF)
						s = -0x7FFF;
					if (s > 0x7FFF)
						s = 0x7FFF;
					
					//Write sample
					decode[i] = s;
					older = old;
					old = s;
				}
			}
			
			void Decoder::DecodeSector8(uint8_t header, const uint8_t (&block)[112], int block_i, int16_t (&decode)[28]) //XA ADPCM 8-bit
			{
				//Read header
				int shift = 8 - (header & 0x0F);
				int filter = (header & 0x30) >> 4;
				
				int f0 = filter_pos[filter];
				int f1 = filter_neg[filter];
				
				//Decode nibbles
				const uint8_t *blockp = block + (block_i ^ 0x3);
				
				for (int i = 0; i < 28; i++)
				{
					//Decode sample
					int t = *blockp;
					blockp += 4;
					
					int s = (t << shift) + ((old * f0 + older * f1 + 32) / 64);
					if (s < -0x7FFF)
						s = -0x7FFF;
					if (s > 0x7FFF)
						s = 0x7FFF;
					
					//Write sample
					decode[i] = s;
					older = old;
					old = s;
				}
			}
		}
	}
}
