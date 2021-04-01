/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
//#define __EMSCRIPTEN__
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL.h>
#include <stdio.h>
#include <string>

#include "../emulator/cpu.hpp"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

CPU *emuCPU;

//The window we'll be rendering to
SDL_Window *gWindow = NULL;

//The window renderer
SDL_Renderer *gRenderer = NULL;

//Current displayed texture
SDL_Texture *gTexture = NULL;

uint16_t mappedKeys[16] = {
	SDLK_x,										   //0
	SDLK_1, SDLK_2, SDLK_3,						   // 1, 2, 3
	SDLK_q, SDLK_w, SDLK_e,						   // 4, 5, 6
	SDLK_a, SDLK_s, SDLK_d,						   // 7, 8, 9
	SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v //a, b, c, d, e, f
};

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Create Texture
				gTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH / 10, SCREEN_HEIGHT / 10);
			}
		}
	}

	return success;
}

void close()
{
	//Free loaded image
	SDL_DestroyTexture(gTexture);
	gTexture = NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void runTick()
{
	CPU *cpu = emuCPU;

	//Process Command
	WORD opcode = cpu->fetch();
	cpu->execute(opcode);
	cpu->decrementDelayTimer(1);
	cpu->decrementSoundTimer(1);

	//Clear screen
	SDL_RenderClear(gRenderer);

	//Load image at specified path
	int width = SCREEN_WIDTH / 10;
	int height = SCREEN_HEIGHT / 10;

	uint8_t *screen = new uint8_t[width * height * 4];

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			if (cpu->getPixel((h * width) + (w)))
			{
				screen[(h * width * 4) + (w * 4)] = (char)0xff;
				screen[(h * width * 4) + (w * 4) + 1] = (char)0xff;
				screen[(h * width * 4) + (w * 4) + 2] = (char)0xff;
				screen[(h * width * 4) + (w * 4) + 3] = (char)0xff;
			}
			else
			{
				screen[(h * width * 4) + (w * 4)] = 0x00;
				screen[(h * width * 4) + (w * 4) + 1] = 0x00;
				screen[(h * width * 4) + (w * 4) + 2] = 0x00;
				screen[(h * width * 4) + (w * 4) + 3] = 0x00;
			}
		}
	}

	SDL_UpdateTexture(gTexture, NULL, screen, width * 4);
	delete[] screen;

	//Render texture to screen
	SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

	//Update screen
	SDL_RenderPresent(gRenderer);
}

#ifdef __EMSCRIPTEN__
void runEmulator()
{
		//Start CPU
		CPU cpu;
		printf("LOADING ROM - IBM LOGO\n");
		cpu.loadIBM();
		emuCPU = &cpu;


		emscripten_set_main_loop(runTick, 60, 1);
}
#else
void runEmulator()
{
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//Start CPU
	CPU cpu;
	cpu.loadROM("./IBM Logo.ch8");
	//cpu.loadROM("../../roms/TestRoms/IBM Logo.ch8");
	//cpu.loadROM("../../roms/TestRoms/corax89_test_opcode.ch8");
	//cpu.loadROM("../../roms/TestRoms/bc_test.ch8");
	//cpu.loadROM("../../roms/Chip-8 Programs/Random Number Test [Matthew Mikolay, 2010].ch8");
	//cpu.loadROM("../../roms/Chip-8 Games/Space Invaders [David Winter].ch8");
	//cpu.loadROM("../../roms/Chip-8 Games/Pong [Paul Vervalin, 1990].ch8");

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else
			{
				if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
				{
					for (int k = 0; k < 16; k++)
					{
						if (e.key.keysym.sym == mappedKeys[k])
						{
							cpu.setKeyPressed(k, (e.type == SDL_KEYDOWN));
						}
					}
				}
			}
		}

		//Process Command
		WORD opcode = cpu.fetch();
		//printf("%x\n", opcode);
		cpu.execute(opcode);
		cpu.decrementDelayTimer(1);
		cpu.decrementSoundTimer(1);

		//Clear screen
		SDL_RenderClear(gRenderer);

		//Load image at specified path
		int width = SCREEN_WIDTH / 10;
		int height = SCREEN_HEIGHT / 10;

		uint8_t *screen = new uint8_t[width * height * 4];

		for (int h = 0; h < height; h++)
		{
			for (int w = 0; w < width; w++)
			{
				if (cpu.getPixel((h * width) + (w)))
				{
					screen[(h * width * 4) + (w * 4)] = (char)0xff;
					screen[(h * width * 4) + (w * 4) + 1] = (char)0xff;
					screen[(h * width * 4) + (w * 4) + 2] = (char)0xff;
					screen[(h * width * 4) + (w * 4) + 3] = (char)0xff;
				}
				else
				{
					screen[(h * width * 4) + (w * 4)] = 0x00;
					screen[(h * width * 4) + (w * 4) + 1] = 0x00;
					screen[(h * width * 4) + (w * 4) + 2] = 0x00;
					screen[(h * width * 4) + (w * 4) + 3] = 0x00;
				}
			}
		}

		SDL_UpdateTexture(gTexture, NULL, screen, width * 4);
		delete[] screen;

		//Render texture to screen
		SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

		//Update screen
		SDL_RenderPresent(gRenderer);
	}
}
#endif

int main(int argc, char *args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		runEmulator();
	}

	//Free resources and close SDL
	close();

	return 0;
}