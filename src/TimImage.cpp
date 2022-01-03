/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "TimImage.h"

namespace PaperPup
{
	namespace TimImage
	{
		//Tim image class
		//Destructor
		TimImage::~TimImage()
		{
			//Delete buffers
			if (part_tex.data != nullptr)
				delete[] part_tex.data;
			if (part_clut.data != nullptr)
				delete[] part_clut.data;
		}
		
		//Tim interface
		bool TimImage::Read(std::istream &stream)
		{
			//Free previously contained buffers
			if (part_tex.data != nullptr)
			{
				delete[] part_tex.data;
				part_tex.data = nullptr;
			}
			if (part_clut.data != nullptr)
			{
				delete[] part_clut.data;
				part_clut.data = nullptr;
			}
			
			//Read Tim header
			uint8_t tim_header[8]{};
			stream.read((char*)tim_header, 8);
			
			if (tim_header[0] != 0x10 || tim_header[1] != 0x00) //Tag and version
				return true;
			
			bpp = (TimBpp)(tim_header[4] & 0x03);
			
			//Read Tim parts
			auto ReadPart = [&](TimPart &part)
			{
				//Read part header
				uint8_t part_header[12]{};
				stream.read((char*)part_header, 12);
				
				//uint32_t part_length = (part_header[0] << 0) | (part_header[1] << 8) | (part_header[2] << 16) | (part_header[3] << 24);
				part.x = (part_header[4] << 0) | (part_header[5] << 8);
				part.y = (part_header[6] << 0) | (part_header[7] << 8);
				part.w = (part_header[8] << 0) | (part_header[9] << 8);
				part.h = (part_header[10] << 0) | (part_header[11] << 8);
				
				//Read part data
				size_t pixels = part.w * part.h;
				part.data = new uint16_t[pixels]{};
				stream.read((char*)part.data, pixels << 1);
			};
			
			if (tim_header[4] & 0x08)
				ReadPart(part_clut);
			ReadPart(part_tex);
			
			return false;
		}
	}
}
