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

std::vector<std::string> main_arg;

int main(int argc, char *argv[])
{
	//Copy arguments
	char **argvp = argv;
	main_arg.resize(argc);
	for (auto &i : main_arg)
		i = *argvp++;
	
	//Run game
	PaperPup::Game *game;
	
	try
	{
		game = new PaperPup::Game();
		return game->Loop() ? 1 : 0;
	}
	catch (const char *e_what)
	{
		std::cout << "[EXCEPTION CAUGHT] " << e_what << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << "[EXCEPTION CAUGHT] " << e.what() << std::endl;
	};
	
	if (game != nullptr)
		delete game;
}
