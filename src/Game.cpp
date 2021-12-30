/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Game.h"

#include <thread>
#include <chrono>

namespace PaperPup
{
	//Game class
	//Constructor and destructor
	Game::Game()
	{
		
	}
	
	Game::~Game()
	{
		
	}
	
	//Game interface
	bool Game::Loop()
	{
		System::CD::File stream_file = system.GetCD()->FindFile("S2/STAGE2.XA1");
		if (stream_file.stream != nullptr)
			system.GetSPU()->XA_Load(stream_file.stream.get());
		else
			throw "Failed to open test XA";
		
		system.GetSPU()->XA_SetFilter(1, 1);
		system.GetSPU()->XA_Play();
		
		std::this_thread::sleep_for(std::chrono::seconds(300));
		
		return false;
	}
}
