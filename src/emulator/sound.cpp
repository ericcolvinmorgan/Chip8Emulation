#include "sound.hpp"
#include <SDL2/SDL.h>

const int AMPLITUDE = 28000;
const int SAMPLE_RATE = 10000;

Sound::Sound()
{

}

const char* Sound::startAudio()
{
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        return SDL_GetError();
    }
    else
    {
        // Setup SDL
        SDL_AudioSpec spec;
        spec.freq = SAMPLE_RATE;
        spec.format = AUDIO_S16SYS;
        spec.silence = 0;
        spec.channels = 2;
        spec.samples = 2048; //4096; NOTE: 4096 made it pulse in the browser
        spec.callback = this->generateSound;
        spec.userdata = this;

        if (SDL_OpenAudio(&spec, NULL) != 0)
        {
            return SDL_GetError();
        }
    }

    return NULL;
}

void Sound::stopAudio()
{
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

void Sound::generateSound(void *user_data, Uint8 *raw_buffer, int bytes)
{
    SDL_memset(raw_buffer, 0, bytes);
    Sound sound = *(Sound*)user_data;
    short *samples = reinterpret_cast<short *>(raw_buffer);
    size_t numSamples = bytes / sizeof(short);

    // for (size_t i = 0; i < numSamples; ++i)
    // {
    //     samples[i] = sound;
    // }
}