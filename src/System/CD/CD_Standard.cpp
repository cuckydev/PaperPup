/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "CD_Standard.h"

#include <fstream>

namespace System
{
	namespace CD
	{
		//CD standard class
		//Constructor and destructor
		CD_Standard::CD_Standard()
		{
			
		}
		
		CD_Standard::~CD_Standard()
		{
			
		}
		
		//CD interface
		File CD_Standard::FindFile(std::string name)
		{
			std::shared_ptr<std::ifstream> stream = std::make_shared<std::ifstream>("iso/" + name, std::ifstream::binary);
			return {
				stream->is_open() ? stream : nullptr,
				name
			};
		}
	}
}
