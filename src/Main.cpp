/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Main.h"

#include "Game.h"

// Program arguments
std::vector<std::string> main_arg;

// Entry point
int main(int argc, char *argv[])
{
	// Copy arguments
	char **argvp = argv;
	main_arg.resize(argc);
	for (auto &i : main_arg)
		i = *argvp++;
	
	// Run game
	PaperPup::Game *game = nullptr;
	
	int result = -1;
	try
	{
		game = new PaperPup::Game();
		result = game->Loop() ? 1 : 0;
	}
	catch (const std::exception &e)
	{
		std::cout << "[EXCEPTION CAUGHT] " << e.what() << std::endl;
	}
	
	if (game != nullptr)
		delete game;
	return result;
}
