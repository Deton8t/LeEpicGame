#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>

void renderBackground(SDL_Surface *surface)
{
    SDL_LockSurface(surface);

    // srand(time(NULL));
    int num_stars = 150;
    uint8_t *pixelArray = (uint8_t *)surface->pixels;

    for (int i = 0; i < num_stars; i++)
    {
        int x = rand() % 580;
        int y = rand() % 450;
        for (int j = 0; j < 2; j++)
        {
            pixelArray[y * surface->pitch + x * surface->format->BytesPerPixel + 0] = (uint8_t)255;
            pixelArray[y * surface->pitch + x * surface->format->BytesPerPixel + 1] = 255;
            pixelArray[y * surface->pitch + x * surface->format->BytesPerPixel + 2] = 255;
            ++x;
        }
    }
    SDL_UnlockSurface(surface);
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
    window = SDL_CreateWindow("Schmaliga", 200, 200, 600, 480, SDL_WINDOW_SHOWN);
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
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
