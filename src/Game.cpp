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
#include "TimImage.h"

#include "Main.h"

#include "StreamHelper.h"

namespace PaperPup
{
	// Game class
	// Constructor and destructor
	Game::Game()
	{
		// Setup system
		system.GetFrontend()->Window_Set("PaperPup", 320 * 3, 240 * 3);
		system.GetGPU()->Screen_Set(320, 240);
	}
	
	Game::~Game()
	{
		
	}
	
	// Game interface
	bool Game::Loop()
	{
		// Play some music
		{
			std::shared_ptr<std::ifstream> test_xa = system.GetCD()->FindFile("S6/STAGE6.XA1");
			if (test_xa == nullptr)
				throw PaperPup::Exception("Failed to open S1/STAGE1.XA1");
			system.GetSPU()->XA_Load(*test_xa);
			system.GetSPU()->XA_SetFilter(1, 1);
			system.GetSPU()->XA_Play();
		}
		
		// Read INT file
		{
			std::shared_ptr<std::ifstream> test_int = system.GetCD()->FindFile("S1/COMPO01.INT");
			if (test_int == nullptr)
				throw PaperPup::Exception("Failed to open S1/COMPO01.INT");
			IntArchive::IntArchive test_int_archive;
			test_int_archive.Read(*test_int);
			
			for (auto &i : test_int_archive.GetLoadTims())
			{
				TimImage::TimImage image;
				std::vector<uint8_t> *vec = test_int_archive[i];
				
				if (vec != nullptr)
				{
					StreamHelper::imemstream vec_str((const char*)vec->data(), vec->size());
					if (!image.Read(vec_str))
					{
						const TimImage::TimPart &part_tex = image.GetTex();
						if (part_tex.data != nullptr)
							system.GetGPU()->VRAM_Write(part_tex.data, part_tex.x, part_tex.y, part_tex.w, part_tex.h);
						
						const TimImage::TimPart &part_clut = image.GetCLUT();
						if (part_clut.data != nullptr)
							system.GetGPU()->VRAM_Write(part_clut.data, part_clut.x, part_clut.y, part_clut.w, part_clut.h);
					}
				}
			}
		}
		
		// Run game as long as system is running
		while (1)
		{
			// Handle frontend events
			system.GetFrontend()->Input_HandleEvents();
			if (system.GetFrontend()->Input_ShouldClose())
				break;
			
			// Run game
			System::GPU::Quad quad;
			quad.vertex[0].x = -1.0f;
			quad.vertex[0].y = 1.0f;
			quad.vertex[0].u = 0;
			quad.vertex[0].v = 0;
			quad.vertex[1].x = -1.0f;
			quad.vertex[1].y = -1.0f;
			quad.vertex[1].u = 0;
			quad.vertex[1].v = 255;
			quad.vertex[2].x = 1.0f;
			quad.vertex[2].y = 1.0f;
			quad.vertex[2].u = 255;
			quad.vertex[2].v = 0;
			quad.vertex[3].x = 1.0f;
			quad.vertex[3].y = -1.0f;
			quad.vertex[3].u = 255;
			quad.vertex[3].v = 255;
			quad.tpage = 5;
			quad.cpage = System::GPU::VRAM_CPAGEW * 493;
			quad.bpp = System::GPU::Bpp::Index4;
			
			system.GetGPU()->Render_Quad(quad);
			
			// Swap frontend buffers
			system.GetFrontend()->Window_SwapBuffers();
		}
		return false;
	}
}
