#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#undef main

const int screen_w = 640;
const int screen_h = 480;
const int num_stars = 300;

class Player
{
public:
    int x;
    int y;
    SDL_Vertex verts[3]{};

    Player(int x, int y)
    {
        this->x = x;
        this->y = y;
    }

    void update_pos(int x, int y)
    {
        this->x = x;
        this->y = y;
        update_verts();
    }

    /*

    verts[0] = {{(float)x, y + 8.66f}, {255, 255, 255, 255}};
            verts[1] = {{x + 5.0f, (float)y}, {255, 255, 255, 255}};
            verts[2] = {{x - 5.0f, (float)y}, {255, 255, 255, 255}};

    */
private:
    void update_verts()
    {
        verts[0].position = {(float)x, y + 8.66f};
        verts[0].color = {255, 255, 255, 255};
    }
};

class Enemy
{
public:
    SDL_Rect body;

public:
    Enemy(int x, int y, int w, int h)
    {
        body.x = x;
        body.y = y;
        body.w = w;
        body.h = h;
    }
    void updatePos(int x, int y)
    {
        body.x = x;
        body.y = y;
    }
};

void renderBackground(SDL_Surface *surface)
{
    SDL_LockSurface(surface);

    srand(time(NULL));
    uint8_t *pixelArray = (uint8_t *)surface->pixels;

    for (int i = 0; i < num_stars; i++)
    {
        int x = rand() % screen_w;
        int y = rand() % screen_h;
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

void scroll_bkg(SDL_Surface *surface)
{
    SDL_LockSurface(surface);
    uint8_t *pixelArray = (uint8_t *)surface->pixels;
    uint8_t *tmp = new uint8_t[screen_w * 4];

    for (int r = screen_h - 2; r >= 0; r--)
    {
        std::memcpy(tmp, pixelArray + (r * screen_w * 4), screen_w * 4);                                   // stores row i in tmp
        std::memcpy(pixelArray + (r * screen_w * 4), pixelArray + ((r + 1) * screen_w * 4), screen_w * 4); // i + i stored in row i
        std::memcpy(pixelArray + ((r + 1) * screen_w * 4), tmp, screen_w * 4);                             // tmp stored in i + 1
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
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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
