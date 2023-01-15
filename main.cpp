#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <algorithm>
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
        for (int i = 0; i < 3; i++)
        {
            verts[i].color = {255, 255, 255, 255};
            verts[i].tex_coord = {0};
        }
        update_verts();
    }

    void update_pos(int x, int y)
    {
        this->x = std::clamp(x, 0, screen_w);
        this->y = y;

        update_verts();
    }

private:
    void update_verts()
    {
        verts[0].position = {(float)x, y - 25.98f};
        verts[1].position = {x + 15.0f, (float)y};
        verts[2].position = {x - 15.0f, (float)y};
    }
};

/* unfinished class
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
    void render(SDL_Surface *surface)
    {
        SDL_FillRect(surface, &body, SDL_MapRGB(255, 255, 255));
    }
};

class EnemyBasic : public Enemy
{
private:
     bool isBackwards = false;
     int moveDelay = 5;
public:
    EnemyBasic(int x, int y)
    {
        body.x = x;
        body.y = y
        body.w = 20;
        body.h = 20;
    }
    void next_pos()
    {
        if(moveDelay != 0)
        {
            --moveDelay;
            return;
        }
        if(body.x >= screen_w - 20)
        {
            body.y = body.y - 25;
            body.x = screen_w -5;
            isBackwards = true;
        }
        if(body.x <= 0)
        {
            body.y = body.y -25
            body.x = 5;
            isBackwards = false;
        }
        if(isBackwards)
        {
            body.x = body.x - 20;
        }
        else()
        {
            body.x = body.x + 20;

        }
    }
};
*/
class Missile
{
public:
    Missile(int x, int y, int speed)
    {
        this->x = x;
        this->y = y;
        this->speed = speed;
        active = true;
    }

    Missile(int x, int y)
    {
        this->x = x;
        this->y = y;
        this->speed = 1;
        active = true;
    }

    void next_pos()
    {
        if (y - speed > 0)
        {
            y -= speed;
        }
        else
        {
            active = false;
        }
    }

    bool is_active()
    {
        return active;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect{x, y - 25, 4, 4};
        SDL_RenderFillRect(renderer, &rect);
    }

    int get_x()
    {
        return x;
    }

    int get_y()
    {
        return y;
    }

private:
    int x;
    int y;
    int speed;
    bool active;
};

void renderBackground(uint8_t *pixelArray)
{
    srand(time(NULL));

    for (int i = 0; i < num_stars; i++)
    {
        int x = rand() % screen_w;
        int y = rand() % screen_h;
        for (int j = 0; j < 2; j++)
        {
            pixelArray[y * screen_w * 4 + x * 4 + 0] = 255;
            pixelArray[y * screen_w * 4 + x * 4 + 1] = 255;
            pixelArray[y * screen_w * 4 + x * 4 + 2] = 255;
            ++x;
        }
    }
}

void scroll_bkg(uint8_t *pixelArray)
{
    uint8_t *tmp = new uint8_t[screen_w * 4];

    for (int r = screen_h - 2; r >= 0; r--)
    {
        std::memcpy(tmp, pixelArray + (r * screen_w * 4), screen_w * 4);                                   // stores row i in tmp
        std::memcpy(pixelArray + (r * screen_w * 4), pixelArray + ((r + 1) * screen_w * 4), screen_w * 4); // i + i stored in row i
        std::memcpy(pixelArray + ((r + 1) * screen_w * 4), tmp, screen_w * 4);                             // tmp stored in i + 1
    }

    delete[] tmp;
}

int main()
{

    uint8_t *pixels = new uint8_t[screen_w * screen_h * 4]();
    SDL_Window *window = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Renderer *renderer = nullptr;
    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);

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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);

    renderBackground(pixels);
    SDL_UpdateTexture(texture, NULL, pixels, screen_w * 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    Player player{screen_w / 2, screen_h - 65};
    std::vector<Missile> missiles;

    clock_t last_missile = 0;

    int counter = 0;
    bool gameIsRunning = true;
    bool isMovingRight = false;
    bool isMovingLeft = false;
    while (gameIsRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameIsRunning = false;
            }
            if (keyboard[SDL_SCANCODE_LEFT])
            {
                player.update_pos(player.x - 5, player.y);
            }
            else if (keyboard[SDL_SCANCODE_RIGHT])
            {
                player.update_pos(player.x + 5, player.y);
            }
            if (keyboard[SDL_SCANCODE_UP])
            {
                if ((double)(clock() - last_missile) / CLOCKS_PER_SEC > 0.5)
                {
                    missiles.push_back({player.x, player.y});
                    last_missile = clock();
                }
            }
        }

        // update bullets
        for (int i = 0; i < missiles.size(); i++)
        {
            if (!missiles[i].is_active())
            {
                missiles.erase(missiles.begin() + i);
            }
            else
            {
                missiles[i].next_pos();
            }
        }

        SDL_UpdateWindowSurface(window);
        scroll_bkg(pixels);
        SDL_UpdateTexture(texture, nullptr, pixels, screen_w * 4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderGeometry(renderer, nullptr, player.verts, 3, nullptr, 0);

        for (auto &missile : missiles)
        {
            missile.draw(renderer);
        }
        SDL_RenderPresent(renderer);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        counter++;
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
