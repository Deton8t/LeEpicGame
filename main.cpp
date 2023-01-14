#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#undef main

const int screen_w = 640;
const int screen_h = 480;

void renderBackground(SDL_Surface *surface)
{
    SDL_LockSurface(surface);

    srand(time(NULL));
    uint8_t *pixelArray = (uint8_t *)surface->pixels;
    
    for (int r = 0; r < screen_h; r++)
    {
        for (int x = 0; x < screen_w; x++)
        {
            if (rand() % 1500 == 0)
            {
                ((uint32_t*)pixelArray)[x + (screen_w * r)] = (uint32_t)-1;
                ((uint32_t*)pixelArray)[x + (screen_w * r) + 1] = (uint32_t)-1;
            }
        }
    }

    SDL_UnlockSurface(surface);
}

void scroll_bkg(SDL_Surface* surface)
{
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*)surface->pixels;
    uint8_t* tmp = new uint8_t[screen_w * 4];

    for (int r = screen_h - 2; r >= 0; r--)
    {
        std::memcpy(tmp, pixelArray + (r * screen_w * 4), screen_w * 4); //stores row i in tmp
        std::memcpy(pixelArray + (r * screen_w * 4), pixelArray + ((r + 1) * screen_w * 4), screen_w * 4); // i + i stored in row i
        std::memcpy(pixelArray + ((r + 1) * screen_w * 4), tmp, screen_w * 4); // tmp stored in i + 1
    }

    for (int x = 0; x < screen_w; x++)
    {
        if (rand() % 1500 == 0)
        {
            ((uint32_t*)pixelArray)[x] = (uint32_t)-1;
            ((uint32_t*)pixelArray)[x + 1] = (uint32_t)-1;
        }
    }

    SDL_UnlockSurface(surface);

    delete[] tmp;
}

int main()
{
    SDL_Window *window = nullptr;
    SDL_Surface *screen;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Initializiton Failed \n"
                  << SDL_GetError();
    }
    else
    {
        std::cout << "Video initialized properly\n";
    }
    window = SDL_CreateWindow("Schmaliga", 200, 200, screen_w, screen_h, SDL_WINDOW_SHOWN);
    screen = SDL_GetWindowSurface(window);

    renderBackground(screen);
    SDL_UpdateWindowSurface(window);

    bool gameIsRunning = true;
    while (gameIsRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameIsRunning = false;
            }
            
        }

        scroll_bkg(screen);
        SDL_UpdateWindowSurface(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
