#pragma once

#include <SDL2/SDL_audio.h>

class Sound
{
public:
	Sound();
	~Sound() {};
	const char* startAudio();
	void stopAudio();

private:
    int test;
	static void generateSound(void *user_data, Uint8 *buffer, int bytes);
};