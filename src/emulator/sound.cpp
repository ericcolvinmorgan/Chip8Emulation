#include "sound.hpp"
#include <cmath>
#include <SDL2/SDL.h>
#include <random>

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;

Sound::Sound()
{
}

const char *Sound::startAudio(bool *triggerSound)
{
    //const double angle = (std::acos(-1) * 2 * FREQUENCY) / (2048 * 1);
    double v = 0;
    for(int i = 0; i < 1024; i++)
    {
        //Initialize
        v += 123.471; //B2
        beep[i] = AMPLITUDE * std::sin(v * 2 * M_PI / FREQUENCY);// * sound.soundTriggered();
    }

    if (SDL_Init(SDL_INIT_AUDIO) != 0)
    {
        return SDL_GetError();
    }
    else
    {
        // Setup SDL
        SDL_AudioSpec spec;
        spec.freq = FREQUENCY;
        spec.format = AUDIO_S16SYS;
        spec.channels = 1;
        spec.samples = 1024;
        spec.callback = this->generateSound;
        spec.userdata = this;

        if (SDL_OpenAudio(&spec, NULL) != 0)
        {
            return SDL_GetError();
        }

        _triggerSound = triggerSound;
        SDL_PauseAudio(0);
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
    std::random_device rd;
    std::mt19937 randomEngine = std::mt19937(rd());
    std::uniform_int_distribution<uint8_t> randomDistribution(SDL_MIN_UINT8, SDL_MAX_UINT8);

    Sound sound = *(Sound *)user_data;
    short *samples = reinterpret_cast<short *>(raw_buffer);
    size_t numSamples = bytes / sizeof(short);
    const double pi = std::acos(-1);

    if(sound.soundTriggered())
    {
        SDL_memcpy(raw_buffer, sound.beep, bytes);
    }
    else
    {
        SDL_memset(raw_buffer, 0, bytes);
    }
}