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
		//Constructor and destructor
		TimImage::~TimImage()
		{
			//Delete buffers
			if (tim_tex != nullptr)
				delete[] tim_tex;
			if (tim_clut != nullptr)
				delete[] tim_clut;
		}
		
		//Tim interface
		bool TimImage::Read(std::istream &stream)
		{
			//Free previously contained buffers
			if (tim_tex != nullptr)
			{
				delete[] tim_tex;
				tim_tex = nullptr;
			}
			if (tim_clut != nullptr)
			{
				delete[] tim_clut;
				tim_clut = nullptr;
			}
			
			return false;
		}
	}
}
