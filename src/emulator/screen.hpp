#pragma once

#include <bitset>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "constants.hpp"

class Screen
{
public:
	Screen();
	~Screen();
    const std::bitset<64 * 32> getPixels() { return _pixels; };
    int initialize(const char* error);
    void refreshScreen(const std::bitset<CHIP_SCREEN_WIDTH * CHIP_SCREEN_HEIGHT> *const pixels);
    void printDebugInfo(int x, int y, const char *message);

private:
    std::bitset<64 * 32> _pixels;
    SDL_Renderer *_renderer = NULL;
    SDL_Window *_window = NULL;
    SDL_Texture *_texture = NULL;
    TTF_Font *_font = NULL;    
};