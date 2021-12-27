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
		System::CD::File stream_file = system.GetCD()->FindFile("S1/STAGE1.XA1");
		system.GetSPU()->XA_Play(stream_file.stream);
		
		std::this_thread::sleep_for(std::chrono::seconds(3));
		return false;
	}
}
