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
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>
#include <cstdint>

namespace PaperPup
{
	namespace IntArchive
	{
		//Int archive class
		class IntArchive
		{
			private:
				//Archive files
				std::vector<std::string> load_tims;
				std::vector<std::pair<std::string, std::string>> load_vabs;
				std::unordered_map<std::string, std::vector<uint8_t>> files;
				
			public:
				//Int interface
				bool Read(std::istream &stream);
				
				//Gets and sets
				const std::vector<std::string> &GetLoadTims() { return load_tims; }
				const std::vector<std::pair<std::string, std::string>> &GetLoadVabs() { return load_vabs; }
				const std::unordered_map<std::string, std::vector<uint8_t>> &GetFiles() { return files; }
				
				//Array access
				const std::vector<uint8_t>* operator[](std::string name) const
				{
					auto file_find = files.find(name);
					if (file_find == files.end())
						return nullptr;
					return &file_find->second;
				}
				std::vector<uint8_t>* operator[](std::string name)
				{
					auto file_find = files.find(name);
					if (file_find == files.end())
						return nullptr;
					return &file_find->second;
				}
		};
	}
}
