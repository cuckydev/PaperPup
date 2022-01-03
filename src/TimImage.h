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

#include "Game.h"

namespace PaperPup
{
	namespace TimImage
	{
		//Tim image class
		class TimImage
		{
			private:
				//Parent game
				Game &game;
				
				//Tim information
				uint16_t *tim_clut = nullptr, *tim_tex = nullptr;
				
			public:
				//Constructor and destructor
				TimImage(Game &_game) : game(_game) {}
				~TimImage();
				
				//Tim interface
				bool Read(std::istream &stream);
		};
	}
}
