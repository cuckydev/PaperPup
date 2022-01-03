/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <streambuf>
#include <istream>

namespace StreamHelper
{
	//Memory stream
	struct membuf : std::streambuf
	{
		membuf(char const* base, size_t size)
		{
			char *p(const_cast<char*>(base));
			this->setg(p, p, p + size);
		}
	};
	struct imemstream : virtual membuf, std::istream
	{
		imemstream(char const* base, size_t size) : membuf(base, size), std::istream(static_cast<std::streambuf*>(this)) {}
	};
}
