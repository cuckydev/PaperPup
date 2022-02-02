/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include "CD.h"

namespace System
{
	namespace CD
	{
		// CD standard class
		class CD_Standard : public CD
		{
			public:
				// Constructor and destructor
				CD_Standard();
				~CD_Standard() override;
				
				// CD interface
				std::shared_ptr<std::ifstream> FindFile(std::string name) override;
		};
	}
}
