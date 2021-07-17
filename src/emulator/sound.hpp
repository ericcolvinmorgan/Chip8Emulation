#pragma once

#include <SDL2/SDL_audio.h>

class Sound
{
public:
	Sound();
	~Sound() {};
	const char* startAudio(bool *triggerSound);
	void stopAudio();
	bool soundTriggered() { return *_triggerSound; }
	short beep[1024] = { 0 };

private:
    bool *_triggerSound;
	static void generateSound(void *user_data, Uint8 *buffer, int bytes);
};