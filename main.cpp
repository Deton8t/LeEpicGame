#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <string>
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

class Enemy
{
public:
    Enemy(int x, int y)
    {
        active = true;
        body.x = x;
        body.y = y;
        body.w = 20;
        body.h = 20;
    }

    void draw(SDL_Renderer *renderer)
    {
        SDL_RenderFillRect(renderer, &body);
    }

    void next_pos()
    {
        if (moveDelay != 0)
        {
            --moveDelay;
            return;
        }
        else
        {
            moveDelay = 10;
        }
        if (body.x >= screen_w - 20)
        {
            if (body.y - 25 >= screen_h)
            {
                active = false;
                return;
            }
            body.y = body.y + 25;
            body.x = screen_w - 5;
            isBackwards = true;
        }
        if (body.x <= 0)
        {
            if (body.y - 25 >= screen_h)
            {
                active = false;
                return;
            }
            body.y = body.y + 25;
            body.x = 5;
            isBackwards = false;
        }
        if (isBackwards)
        {
            body.x = body.x - 20;
        }
        else
        {
            body.x = body.x + 20;
        }


    }

    bool is_active()
    {
        return active;
    }

    int get_x()
    {
        return body.x;
    }

    int get_y()
    {
        return body.y;
    }

private: 
    bool isBackwards = false;
    int moveDelay = 5;
    bool active;
    SDL_Rect body;
};

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
        this->speed = 2;
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
        SDL_Rect rect{ x, y - 25, 6, 6 };
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

void renderBackground(uint8_t* pixelArray)
{
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

void scroll_bkg(uint8_t* pixelArray)
{
    uint8_t* tmp = new uint8_t[screen_w * 4];

    for (int r = screen_h - 2; r >= 0; r--)
    {
        std::memcpy(tmp, pixelArray + (r * screen_w * 4), screen_w * 4);                                   // stores row i in tmp
        std::memcpy(pixelArray + (r * screen_w * 4), pixelArray + ((r + 1) * screen_w * 4), screen_w * 4); // i + i stored in row i
        std::memcpy(pixelArray + ((r + 1) * screen_w * 4), tmp, screen_w * 4);                             // tmp stored in i + 1
    }

    delete[] tmp;
}

//i dont like this, it is ineffecient 
void render_text(TTF_Font* font, SDL_Renderer* renderer, const char* text)
{
    int tw = 0;
    int th = 0;
    SDL_Rect text_rect = {0, 0, 0, 0};

    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, {255, 255, 255});
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_QueryTexture(text_texture, nullptr, nullptr, &tw, &th);
    text_rect.w = tw;
    text_rect.h = th;
    
    SDL_RenderCopy(renderer, text_texture, &text_rect, &text_rect);
    
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
    
}

enum Keys { UP, LEFT, RIGHT };

int main()
{
    srand(time(NULL));
    bool keysDown[3]{ false };
    uint8_t* pixels = new uint8_t[screen_w * screen_h * 4]();
    SDL_Window* window = nullptr;
    SDL_Texture* texture = nullptr;
    SDL_Renderer* renderer = nullptr;

    Mix_Chunk* shoot_sound = nullptr;
    Mix_Chunk* explode_sound = nullptr;

    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << "Initializiton Failed \n"
            << SDL_GetError();
    }
    else
    {
        std::cout << "Video and audio initialized properly\n";
    }

    //rate, format, channels, chunksize
    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        std::cout << "SDL_mixer failed to init, SDL_mixer Error: " << Mix_GetError();
    }

    if (TTF_Init() == -1)
    {
        std::cout << "error in init ttf: " << TTF_GetError() << '\n';
    }

    TTF_Font* font = TTF_OpenFont("./Ubuntu-M.ttf", 50);
    

    shoot_sound = Mix_LoadWAV("./shooting_sfx.wav");
    explode_sound = Mix_LoadWAV("./explode_sfx.wav");

    window = SDL_CreateWindow("Schmaliga", 200, 200, screen_w, screen_h, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);

    renderBackground(pixels);
    SDL_UpdateTexture(texture, NULL, pixels, screen_w * 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    Player player{ screen_w / 2, screen_h - 65 };
    std::vector<Missile> missiles;
    std::vector<Enemy> enemies;

    clock_t last_missile = 0;

    int score = 0;
    int counter = 0;
    bool gameIsRunning = true;
    bool isMovingRight = false;
    bool isMovingLeft = false;
    while (gameIsRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            keysDown[Keys::LEFT] = false;
            keysDown[Keys::RIGHT] = false;
            keysDown[Keys::UP] = false;
            if (event.type == SDL_QUIT)
            {
                gameIsRunning = false;
            }
            if (keyboard[SDL_SCANCODE_LEFT])
            {
                keysDown[Keys::LEFT] = true;
            }
            else if (keyboard[SDL_SCANCODE_RIGHT])
            {
                keysDown[Keys::RIGHT] = true;
            }
            if (keyboard[SDL_SCANCODE_UP])
            {
                keysDown[Keys::UP] = true;
            }
            
        }
        if(keysDown[Keys::LEFT])
        {
            player.update_pos(player.x - 2, player.y);
        }
        else if(keysDown[Keys::RIGHT])
        {
            player.update_pos(player.x + 2, player.y);
        }
        if(keysDown[Keys::UP])
        {
            if ((double)(clock() - last_missile) / CLOCKS_PER_SEC > 0.25)
            {
                missiles.push_back({ player.x, player.y });
                last_missile = clock();
                Mix_PlayChannel(-1, shoot_sound, 0);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
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
        

        if (counter % 100 == 0)
        {
            enemies.push_back({ 5, 5 });
        }

        for (int i = 0; i < enemies.size(); i++)
        {
            if (!enemies[i].is_active())
            {
                enemies.erase(enemies.begin() + i);
            }
            else
            {
                enemies[i].next_pos();
                
            }

        }

        for (int i = 0; i < enemies.size(); i++)
        {
            for (int j = 0; j < missiles.size(); j++)
            {
                int xDisplacement = enemies[i].get_x() - missiles[j].get_x();
                int yDisplacement = enemies[i].get_y() - missiles[j].get_y();
                if (xDisplacement <= 20 && xDisplacement >= 0 && yDisplacement >= -20 && yDisplacement <= 0)
                {
                    enemies.erase(enemies.begin() + i);
                    missiles.erase(missiles.begin() + j);
                    Mix_PlayChannel(-1, explode_sound, 0);
                    score++;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (auto& missile : missiles)
        {
            missile.draw(renderer);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto& enemy : enemies)
        {
            enemy.draw(renderer);
        }

        render_text(font, renderer, std::to_string(score).c_str());
        SDL_RenderGeometry(renderer, nullptr, player.verts, 3, nullptr, 0);

        SDL_RenderPresent(renderer);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        counter++;
    }
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();

    return 0;
}
