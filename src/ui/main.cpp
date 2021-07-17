//Using SDL and standard IO
//#define __EMSCRIPTEN__
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>
#include "../emulator/cpu.hpp"
#include "../emulator/screen.hpp"
#include "../emulator/sound.hpp"

uint16_t mappedKeys[16] = {
	SDLK_x,										   //0
	SDLK_1, SDLK_2, SDLK_3,						   // 1, 2, 3
	SDLK_q, SDLK_w, SDLK_e,						   // 4, 5, 6
	SDLK_a, SDLK_s, SDLK_d,						   // 7, 8, 9
	SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v //a, b, c, d, e, f
};

int totalFrames = 0;
auto startTime = std::chrono::high_resolution_clock::now();
double currentFPS = 60.;

CPU *gCPU;
Screen *gScreen;
Sound *gSound;
int gTimerSpeed = 60;
int gTargetSpeed = 500;
bool gSoundOn = 0;
bool gPaused = 0;
bool gQuit = false;

void runCycle()
{
	if (!gPaused)
	{
		//Event handler
		SDL_Event e;

		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			// User requests quit
			if (e.type == SDL_QUIT)
			{
				gQuit = true;
			}
			else
			{
				if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
				{
					for (int k = 0; k < 16; k++)
					{
						if (e.key.keysym.sym == mappedKeys[k])
						{
							gCPU->setKeyPressed(k, (e.type == SDL_KEYDOWN));
						}
					}
				}
			}
		}

		//Process Command
		int clockTicks = gTargetSpeed / currentFPS;
		for (int i = 0; i < clockTicks; i++)
		{
			WORD opcode = gCPU->fetch();
			gCPU->execute(opcode);
		}

		//Update Timers
		int timerTicks = gTimerSpeed / currentFPS;
		for (int i = 0; i < timerTicks; i++)
		{
			gCPU->decrementDelayTimer(1);
			gCPU->decrementSoundTimer(1);
		}

		gScreen->refreshScreen(gCPU->getScreenPixels());
	}

	totalFrames++;
	double currentFPS = totalFrames / std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - startTime).count();

#ifdef __EMSCRIPTEN__
	EM_ASM({
		Interface.updateScreen($0);
	},
		   gCPU->getRegisters());
#endif
}

#ifdef __EMSCRIPTEN__
void runEmulator()
{
	gCPU->loadROM("LOADROM.ch8");
	EM_ASM(
		{
			Interface.indexRegister = new Uint16Array(
				Module.HEAPU8.buffer,
				$0,
				1);

			Interface.programCounter = new Uint16Array(
				Module.HEAPU8.buffer,
				$1,
				1);

			Interface.registers = new Uint8Array(
				Module.HEAPU8.buffer,
				$2,
				16);

			Interface.delayTimer = new Uint8Array(
				Module.HEAPU8.buffer,
				$3,
				1);

			Interface.soundTimer = new Uint8Array(
				Module.HEAPU8.buffer,
				$4,
				1);
		},
		gCPU->getIndexRegister(), gCPU->getProgramCounter(), gCPU->getRegisters(), gCPU->getDelayTimer(), gCPU->getSoundTimer());

	emscripten_set_main_loop(runCycle, 0, 1);
}
#else
void runEmulator()
{
	//Main loop flag
	gQuit = false;
	gCPU->loadROM("./roms/BRIX.ch8");

	//While application is running
	while (!gQuit)
	{
		runCycle();
	}
}
#endif

extern "C"
{
	void loadROM(const BYTE *romData)
	{
		gCPU->loadROM(romData);
	}

	void setTargetSpeed(int targetSpeed)
	{
		gTargetSpeed = targetSpeed;
	}

	void setSoundOn(int soundOn)
	{
		gSoundOn = soundOn;
		gSound->stopAudio();
		if(gSoundOn)
		{
			gSound->startAudio((bool *)gCPU->getSoundTimer());
		}
	}

	void setPaused(int paused)
	{
		gPaused = paused;
	}

	void dumpCPUDetail(BYTE *const detail)
	{
		auto registers = gCPU->getRegisters();
		std::copy(registers, &registers[0xf], detail);
	}
}

int main(int argc, char *args[])
{
	char *error;

	CPU cpu;
	gCPU = &cpu;

	Screen screen;
	if (screen.initialize(error) == 0)
	{
		gScreen = &screen;

		Sound sound;
		
		if(gSoundOn)
			sound.startAudio((bool *)cpu.getSoundTimer());

		gSound = &sound;

		runEmulator();
	}

	gCPU = NULL;
	gScreen = NULL;
	gSound = NULL;

	return 0;
}