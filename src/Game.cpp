/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Game.h"

#include "IntArchive.h"

#include "Main.h"

namespace PaperPup
{
	//Game class
	//Constructor and destructor
	Game::Game()
	{
		//Setup system
		system.GetFrontend()->Window_Set("PaperPup", 320 * 3, 240 * 3);
		system.GetGPU()->Screen_Set(320, 240);
	}
	
	Game::~Game()
	{
		
	}
	
	//Game interface
	bool Game::Loop()
	{
		//Play some music
		{
			std::shared_ptr<std::istream> test_xa = system.GetCD()->FindFile("S9/STAGE9.XA1");
			if (test_xa == nullptr)
				throw PaperPup::Exception("Failed to open S1/STAGE1.XA1");
			system.GetSPU()->XA_Load(*test_xa);
			system.GetSPU()->XA_SetFilter(1, 1);
			system.GetSPU()->XA_Play();
		}
		
		//Read INT file
		{
			std::shared_ptr<std::istream> test_int = system.GetCD()->FindFile("S1/COMPO01.INT");
			if (test_int == nullptr)
				throw PaperPup::Exception("Failed to open S1/COMPO01.XA1");
			IntArchive::IntArchive test_int_archive(*this);
			test_int_archive.Read(*test_int);
		}
		
		//Run game as long as system is running
		while (1)
		{
			//Handle frontend events
			system.GetFrontend()->Input_HandleEvents();
			if (system.GetFrontend()->Input_ShouldClose())
				break;
			
			//Run game
			
			//Swap frontend buffers
			system.GetFrontend()->Window_SwapBuffers();
		}
		return false;
	}
}
