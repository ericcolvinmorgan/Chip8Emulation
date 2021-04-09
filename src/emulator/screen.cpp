//Using SDL and standard IO
#include <SDL2/SDL.h>
#include <string>
#include "screen.hpp"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

Screen::Screen()
{
}

int Screen::initialize(const char *error)
{
    int init = SDL_Init(SDL_INIT_VIDEO);

    //Initialize SDL
    if (init < 0)
    {
        error = SDL_GetError();
        return init;
    }
    else
    {
        //Set texture filtering to linear
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

        //Create window
        _window = SDL_CreateWindow("Chip 8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (_window == NULL)
        {
            error = SDL_GetError();
            return 1;
        }
        else
        {
            //Create renderer for window
            _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
            if (_renderer == NULL)
            {
                error = SDL_GetError();
                return 2;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(_renderer, 251, 120, 67, 1);

                //Create Texture
                _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH / 10, SCREEN_HEIGHT / 10);

                //Init TTF
                TTF_Init();
                _font = TTF_OpenFont("fontset.ttf", 12);
                if (_font == NULL)
                {
                    error = "Font not found!";
                    return 3;
                }
            }
        }
    }

    return 0;
}

void Screen::refreshScreen(const std::bitset<CHIP_SCREEN_WIDTH * CHIP_SCREEN_HEIGHT> *const pixels)
{
    //Clear screen
    SDL_RenderClear(_renderer);

    //Load image at specified path
    int width = SCREEN_WIDTH / 10;
    int height = SCREEN_HEIGHT / 10;

    uint8_t *screen = new uint8_t[width * height * 4];

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            if (pixels->test((h * width) + (w)))
            {                
                screen[(h * width * 4) + (w * 4)] = 1;
                screen[(h * width * 4) + (w * 4) + 1] = 67;
                screen[(h * width * 4) + (w * 4) + 2] = 120;
                screen[(h * width * 4) + (w * 4) + 3] = 251;
                // screen[(h * width * 4) + (w * 4)] = 0xff;
                // screen[(h * width * 4) + (w * 4) + 1] = 0xff;
                // screen[(h * width * 4) + (w * 4) + 2] = 0xff;
                // screen[(h * width * 4) + (w * 4) + 3] = 0xff;
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

    SDL_UpdateTexture(_texture, NULL, screen, width * 4);
    delete[] screen;

    //Render texture to screen
    SDL_RenderCopy(_renderer, _texture, NULL, NULL);

    //Update screen
    SDL_RenderPresent(_renderer);
}

void Screen::printDebugInfo(int x, int y, const char *message)
{
    SDL_Rect rect;
    SDL_Surface *surface;
    SDL_Texture *texture;

    SDL_Color color{255, 255, 255, 255};

    surface = TTF_RenderText_Solid(_font, message, color);
    texture = SDL_CreateTextureFromSurface(_renderer, surface);
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(_renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

Screen::~Screen()
{
    SDL_DestroyTexture(_texture);
    _texture = NULL;
    SDL_DestroyRenderer(_renderer);
    _renderer = NULL;
    SDL_DestroyWindow(_window);
    _window = NULL;

    SDL_Quit();
}