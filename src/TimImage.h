/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <iostream>
#include <cstdint>

namespace PaperPup
{
	namespace TimImage
	{
		// Tim enums
		enum TimBpp
		{
			Index4,
			Index8,
			RGBX16,
			RGBX24
		};
		
		// Tim part structure
		struct TimPart
		{
			uint16_t *data = nullptr;
			unsigned int x, y, w, h;
		};
		
		// Tim image class
		class TimImage
		{
			private:
				// Tim information
				TimBpp bpp;
				
				TimPart part_tex, part_clut;
				
			public:
				// Destructor
				~TimImage();
				
				// Tim interface
				bool Read(std::istream &data);
				
				TimBpp GetBpp() { return bpp; }
				const TimPart &GetTex() { return part_tex; }
				const TimPart &GetCLUT() { return part_clut; }
		};
	}
}
