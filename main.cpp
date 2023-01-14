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
            verts[i].color = { 255, 255, 255, 255 };
            verts[i].tex_coord = { 0 };
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
        verts[0].position = { (float)x, y - 25.98f };
        verts[1].position = { x + 15.0f, (float)y };
        verts[2].position = { x - 15.0f, (float)y };
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
class Bullet
{
public:
    Bullet(int x, int y, int speed)
    {
        this->x = x;
        this->y = y;
        this->speed = speed;
        active = true;
    }

    Bullet(int x, int y)
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

    void draw(SDL_Renderer* renderer)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect rect{ x, y - 25, 4, 4 };
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

void renderBackground(SDL_Surface* surface)
{
    SDL_LockSurface(surface);

    srand(time(NULL));
    uint8_t* pixelArray = (uint8_t*)surface->pixels;

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

void scroll_bkg(SDL_Surface* surface)
{
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*)surface->pixels;
    uint8_t* tmp = new uint8_t[screen_w * 4];

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
    SDL_Window* window = nullptr;
    SDL_Surface* screen;
    SDL_Texture* texture;
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
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    renderBackground(screen);
    SDL_UpdateWindowSurface(window);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    texture = SDL_CreateTextureFromSurface(renderer, screen);
    Player player{ screen_w / 2, screen_h - 65 };
    std::vector<Bullet> bullets;

    clock_t last_bullet = 0;
    
    int counter = 0;
    bool gameIsRunning = true;
    while (gameIsRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    gameIsRunning = false;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        player.update_pos(player.x - 5, player.y);
                        break;
                    case SDLK_RIGHT:
                        player.update_pos(player.x + 5, player.y);
                        break;
                    case SDLK_UP:
                        if ((double)(clock() - last_bullet) / CLOCKS_PER_SEC > 0.001 )
                        {
                            bullets.push_back({ player.x, player.y });
                        }
                        last_bullet = clock();
                        break;
                    default:
                        break;
                    }
            }
        }

        //update bullets
        for (int i = 0; i < bullets.size(); i++)
        {
            if (!bullets[i].is_active())
            {
                bullets.erase(bullets.begin() + i);
            }
            else
            {
                bullets[i].next_pos();
            }
        }

        

        scroll_bkg(screen);
        SDL_UpdateTexture(texture, nullptr, screen->pixels, screen->pitch);
        SDL_RenderClear(renderer);
        //render bullets
        
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderGeometry(renderer, nullptr, player.verts, 3, nullptr, 0);

        for (auto& bullet : bullets)
        {
            bullet.draw(renderer);
        }
        SDL_RenderPresent(renderer);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        counter++;
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
