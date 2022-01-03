/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "IntArchive.h"

#include <algorithm>
#include <iterator>

namespace PaperPup
{
	namespace IntArchive
	{
		//Int archive class
		//Int interface
		bool IntArchive::Read(std::istream &stream)
		{
			//Clear previous files
			load_tims.clear();
			load_vabs.clear();
			files.clear();
			
			//Read block by block
			while (1)
			{
				//Check if stream has prematurely reached end
				if (stream.eof())
					return true;
				
				//Read block header
				uint8_t block_header[4 << 11];
				stream.read((char*)block_header, 4 << 11);
				
				uint32_t block_type = (block_header[0] << 0) | (block_header[1] << 8) | (block_header[2] << 16) | (block_header[3] << 24);
				uint32_t block_files = (block_header[4] << 0) | (block_header[5] << 8) | (block_header[6] << 16) | (block_header[7] << 24);
				uint32_t block_datasize = (block_header[8] << 0) | (block_header[9] << 8) | (block_header[10] << 16) | (block_header[11] << 24);
				
				//Read block data
				std::vector<uint8_t> block_data(block_datasize << 11);
				auto block_datap = block_data.begin();
				
				stream.read((char*)block_data.data(), block_data.size());
				
				//Read files
				uint8_t *filep = block_header + 0x10;
				
				switch (block_type)
				{
					case 1: //Tim images
					{
						for (uint32_t i = 0; i < block_files; i++)
						{
							//Read file header
							std::string file_name((char*)(filep + 4), 16);
							uint32_t file_size = (filep[0] << 0) | (filep[1] << 8) | (filep[2] << 16) | (filep[3] << 24);
							filep += 0x14;
							
							//Add to load tim list and file list
							load_tims.push_back(file_name);
							files.emplace(std::make_pair(file_name, std::vector<uint8_t>(block_datap, block_datap + file_size)));
							block_datap += file_size;
						}
						break;
					}
					case 2: //Vab sounds
					{
						//Vabs are hardcoded to have 2 files
						std::string file_names[2];
						for (uint32_t i = 0; i < 2; i++)
						{
							//Read file header
							std::string file_name((char*)(filep + 4), 16); file_names[i] = file_name;
							uint32_t file_size = (filep[0] << 0) | (filep[1] << 8) | (filep[2] << 16) | (filep[3] << 24);
							filep += 0x14;
							
							//Add to file list
							files.emplace(std::make_pair(file_name, std::vector<uint8_t>(block_datap, block_datap + file_size)));
							block_datap += file_size;
						}
						
						//Add to load vab list
						load_vabs.emplace_back(file_names[0], file_names[1]);
						break;
					}
					case 3: //Files
					{
						for (uint32_t i = 0; i < block_files; i++)
						{
							//Read file header
							std::string file_name((char*)(filep + 4), 16);
							uint32_t file_size = (filep[0] << 0) | (filep[1] << 8) | (filep[2] << 16) | (filep[3] << 24);
							filep += 0x14;
							
							//Add to file list
							files.emplace(std::make_pair(file_name, std::vector<uint8_t>(block_datap, block_datap + file_size)));
							block_datap += file_size;
						}
						break;
					}
					case 0xFFFFFFFF: //End of file
					{
						return false;
					}
					default: //Unknown header
					{
						return true;
					}
				}
			}
		}
	}
}
