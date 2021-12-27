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
#include <memory>
#include <string>

namespace System
{
	namespace CD
	{
		//CD file structure
		struct File
		{
			std::shared_ptr<std::istream> stream;
			std::string name;
		};
		
		//CD class
		class CD
		{
			public:
				//Constructor and destructor
				virtual ~CD() {}
				
				//CD interface
				virtual File FindFile(std::string name) = 0;
		};
	}
}
