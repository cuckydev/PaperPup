/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <string>
#include <vector>
#include <stdexcept>

// Program arguments
extern std::vector<std::string> main_arg;

namespace PaperPup
{
	// Exception class
	class Exception : public std::exception
	{
		protected:
			// Exception string
			std::string what_str;
			
		public:
			// Constructor and destructor
			explicit Exception(std::string str) : what_str(str) {}
			explicit Exception(const char *str) : what_str(str) {}
			virtual ~Exception() noexcept {}
			
			// What
			virtual const char *what() const noexcept override { return what_str.c_str(); }
	};
}
