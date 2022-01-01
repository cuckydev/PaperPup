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
		//Setup system
		system.GetFrontend()->SetWindow("PaperPup", 320 * 3, 240 * 3);
		system.GetGPU()->SetScreen(320, 240);
	}
	
	Game::~Game()
	{
		
	}
	
	//Game interface
	bool Game::Loop()
	{
		//Run game as long as system is running
		while (1)
		{
			//Handle frontend events
			system.GetFrontend()->HandleEvents();
			if (system.GetFrontend()->ShouldClose())
				break;
			
			//Run game
			
			//Swap frontend buffers
			system.GetFrontend()->SwapBuffers();
		}
		return false;
	}
}
