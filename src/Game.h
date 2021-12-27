/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "System/System.h"

namespace PaperPup
{
	//Game class
	class Game
	{
		private:
			//System
			System::System system;
			
		public:
			//Constructor and destructor
			Game();
			~Game();
			
			//Game interface
			bool Loop();
			
			//Gets and sets
			System::System &GetSystem() { return system; }
	};
	
}
