/*
 * PaperPup Engine
 * Copyright Regan "CuckyDev" Green 2021-2022
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Game.h"

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
	PaperPup::Game game;
	return game.Loop() ? 1 : 0;
}
