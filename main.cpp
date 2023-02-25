#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
#include <numbers>
#include <cmath>
#include <string>
#include <algorithm>
#undef main

const int screen_w = 640;
const int screen_h = 480;
const int num_stars = 300;
int player_x;
int player_y;

struct Point3D
{
    float x;
    float y;
    int z;
};

enum Keys
{
    UP,
    LEFT,
    RIGHT
};

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
        player_x = this->x;
        player_y = this->y;
    }

    void draw(SDL_Renderer* renderer)
    {
        SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
    }
    
private:
    void update_verts()
    {
        verts[0].position = {(float)x, y - 25.98f};
        verts[1].position = {x + 15.0f, (float)y};
        verts[2].position = {x - 15.0f, (float)y};
    }
};

class Enemy
{
public:
    virtual void draw(SDL_Renderer*) = 0;
    virtual void next_pos() = 0;
    virtual bool is_active() = 0;
    virtual int get_x() = 0;
    virtual int get_y() = 0;
    virtual ~Enemy() = default;
    virtual int get_value() = 0;
};

class EnemyBasic : public Enemy
{
public:
    EnemyBasic(int x, int y) 
    {
        active = true;
        looping = false;
        t = std::numbers::pi/2;
        dir = 1;
        body.x = x;
        body.y = y;
        body.w = 20;
        body.h = 20;
    }

    void draw(SDL_Renderer *renderer) override
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &body);
    }

    void next_pos() override
    {
        double a = 20.0;
        
        if (body.x >= screen_w - 45 && !looping)
        {
            t = std::numbers::pi/2 - 0.01;
            dir = 0;
            body.x -= 2;
            begin_x = body.x;
            begin_y = body.y;
            looping = true;
            
        }
        if (body.x <= 45 && !looping && body.y > 10)
        {   
            t = std::numbers::pi/2 - 0.01;
            dir = 1;
            body.x += 2;
            begin_x = body.x;
            begin_y = body.y;
            looping = true;
        }
        

        if (t > (-std::numbers::pi/2)- 0.2 && looping)
        {
            if (dir == 0)
            {
                body.x = begin_x + (a * std::cos(t));
                body.y = begin_y + (a * -std::sin(t)) - (a * -std::sin(std::numbers::pi/2));
                t -= 0.1;
            }
            else
            {
                body.x = begin_x - (a * std::cos(t));
                body.y = begin_y + (a * -std::sin(t)) - (a * -std::sin(std::numbers::pi/2));
                t -= 0.1;
            }
        }
        else
        {
            if (dir == 0)
            {
                body.x -= 2;
            }
            else
            {
                body.x += 3;
            }
        }
        
        if (t <= (-std::numbers::pi/2) - 0.2)
        {
            looping = false;
        }

        if (body.y >= screen_h)
        {
            active = false;
        }
    }

    bool is_active() override
    {
        return active;
    }

    int get_x() override
    {
        return body.x;
    }

    int get_y() override
    {
        return body.y;
    }

    int get_value() override
    {
        return 1;
    }


private:
    int begin_x;
    int begin_y;
    int dir;
    bool looping;
    double t;
    bool active;
    SDL_Rect body;
};

class Enemy2 : public Enemy
{
public:
    Enemy2(int x, int y)
    {
        active = true;
        body.x = x;
        body.y = y;
        body.w = 20;
        body.h = 20;
    }

    void draw(SDL_Renderer *renderer) override
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderFillRect(renderer, &body);
    }

    void next_pos() override
    {
        if (body.y + 3 >= screen_h)
        {
            active = false;
            return;
        }
        else
        {
            body.y += 3;
        }
    }

    bool is_active() override
    {
        return active;
    }

    int get_x() override
    {
        return body.x;
    }

    int get_y() override
    {
        return body.y;
    }

    int get_value() override
    {
        return 2;
    }

private:
    bool active;
    SDL_Rect body;
};

class Enemy3 : public Enemy
{
public:
    Enemy3(int x, int y)
    {
        active = true;
        first = true;
        angle = 0.0;
        this->x = x;
        this->y = y;
        flip = rand() % 2 == 0;

        int offset = rand() % screen_h / 4;
        if (offset >= screen_h / 8)
        {
            offset -= screen_h / 8;
            offset *= -1;
        }
        loop_y = screen_h / 4 + offset;

        for (int i = 0; i < 3; i++)
        {
            verts[i].color = {255, 255, 255, 255};
            verts[i].tex_coord = {0};
        }
        update_verts();
    }

    void draw(SDL_Renderer *renderer) override
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        //SDL_RenderDrawPoint(renderer, x, y);
    }

    void next_pos() override
    {
        int radius = screen_h / 15;
        //writing std::numbers::pi is starting to annoy me i may replace it
        if((y >= loop_y || !first) && angle < 2.0 * std::numbers::pi)
        {
            if (first)
            {
                begin_x = x;
                begin_y = y;
                first = false;
            }

            if (flip)
            {
                double t = 2.0 * std::numbers::pi - angle;
                x = begin_x + radius * std::cos(t + std::numbers::pi) + radius;
                y = begin_y + radius * std::sin(t + std::numbers::pi);
                update_verts();
                rot(t);
            }
            else
            {
                x = begin_x + (radius * std::cos(angle)) - radius;
                y = begin_y + radius * std::sin(angle);
                update_verts();
                rot(angle);
            }

            angle += 0.1;
        }
        else
        {
           y += 3; 
           update_verts();
        }
        if (y >= screen_h)
        {
            active = false;
        }
    }

    bool is_active() override
    {
        return active;
    }

    int get_x() override
    {
        return x;
    }

    int get_y() override
    {
        return y;
    }

    int get_value() override
    {
        return 3;
    }

private:
    int x;
    int y;
    SDL_Vertex verts[3]{};
    bool flip;
    int loop_y;
    bool first;
    bool active;
    int begin_x;
    int begin_y;
    float angle;

    //x and y are the center of the triangle
    void update_verts()
    {
        verts[0].position = {static_cast<float>(x), y + 12.99f};
        verts[1].position = {x + 5.0f, static_cast<float>(y) - 12.99f};
        verts[2].position = {x - 5.0f, static_cast<float>(y) - 12.99f};
    }

    void rot(double angle)
    {
        for (int i = 0; i < 3; i++)
        {
            double x1 = verts[i].position.x;
            double y1 = verts[i].position.y;
            verts[i].position.x = std::cos(angle) * (x1 - x) - std::sin(angle) * (y1 - y) + x;
            verts[i].position.y = std::sin(angle) * (x1 - x) + std::cos(angle) * (y1 - y) + y;
        }
    }
};

class Enemy4 : public Enemy
{
public:
    Enemy4(int x, int y)
    {
        body.x = x;
        body.y = y;
        body.w = 15;
        body.h = 15;
        active = true;
        start_y = 0;
        t = 0.0;
        start_x = x;
    }

    void draw(SDL_Renderer *renderer) override
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
        SDL_RenderFillRect(renderer, &body);
    }

    void next_pos() override
    {
        if (t <= 1.0)
        {
            int x1 = start_x;
            int y1 = player_y - 75;

            float u = 1.0 - t;
            body.x = round(u*u*start_x + 2*u*t*x1 + t*t*player_x);
            body.y = round(u*u*start_y + 2*u*t*y1 + t*t*player_y - 75);
            t += 0.007;
        }
        
        if (body.y >= screen_h || body.y >= player_y - 12)
        {
            active = false;
        }
    }

    bool is_active() override
    {
        return active;
    }

    int get_x() override
    {
        return body.x;
    }

    int get_y() override
    {
        return body.y;
    }

    int get_value() override
    {
        return 4;
    }

private:
    SDL_Rect body;
    bool active;
    int start_y;
    int start_x; 
    float t;
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
        this->speed = 8;
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
        SDL_Rect rect{x, y - 25, 6, 6};
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

std::vector<Point3D> get_points(int num_pts)
{
    std::vector<Point3D> pts;
    pts.reserve(num_pts);
    for (int i = 0; i < num_pts; i++)
    {
        float x = rand() % screen_w;
        float y = rand() % screen_h;
        int z = rand() % 3;
        pts.push_back({x, y, z});
    }

    return pts;
}

void move_pts(std::vector<Point3D> &pts)
{
    for (Point3D &pt : pts)
    {
        switch (pt.z)
        {
            case 0:
                pt.y += 0.4;
            case 1:
                pt.y += 0.3;
            case 2:
                pt.y += 0.2;
        }

        if (pt.y >= screen_h)
        {
            pt.y = 0;
            pt.x = rand() % screen_w;
            pt.z = rand() % 3;
        }
    }
}

void render_text(TTF_Font *font, SDL_Renderer *renderer, const char *text, uint8_t r, uint8_t g, uint8_t b, int x, int y)
{
    // i dont like this, it is ineffecient
    
    SDL_Rect text_rect = {x, y, 0, 0};
    int tw = 0;
    int th = 0;
    
    SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, {r, g, b});
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

    SDL_QueryTexture(text_texture, nullptr, nullptr, &tw, &th);
    text_rect.w = tw;
    text_rect.h = th;

    SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

double dist(int x1, int y1, int x2, int y2)
{
    return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

int main()
{
    int lives = 3;
    const int default_font_size = 50;
    srand(time(NULL));
    bool keysDown[3]{false};
    uint8_t *pixels = new uint8_t[screen_w * screen_h * 4]();

    const Uint8 *keyboard = SDL_GetKeyboardState(NULL);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << "initialization Failed \n" << SDL_GetError();
    }
    else
    {
        std::cout << "Video and audio initialized properly\n";
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "SDL_mixer failed to init, SDL_mixer Error: " << Mix_GetError();
    }

    if (TTF_Init() == -1)
    {
        std::cout << "error in init ttf: " << TTF_GetError() << '\n';
    }

    TTF_Font *font = TTF_OpenFont("./retro_font.ttf", default_font_size);

    if (font == nullptr)
    {
        std::cout << "error in reading font file\n";
    }

    Mix_Chunk *shoot_sound = Mix_LoadWAV("./fire.wav");
    Mix_Chunk *explode_sound = Mix_LoadWAV("./point_score.wav");

    SDL_Window *window = SDL_CreateWindow("Schmaliga", 200, 200, screen_w, screen_h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);

    std::vector<Point3D> pts = get_points(num_stars);
    for (const Point3D &pt : pts)
    {
        int x = static_cast<int>(pt.x);
        int y = static_cast<int>(pt.y);
        pixels[y * screen_w * 4 + x * 4 + 0] = 255;
        pixels[y * screen_w * 4 + x * 4 + 1] = 255;
        pixels[y * screen_w * 4 + x * 4 + 2] = 255;
        pixels[y * screen_w * 4 + x * 4 + 3] = 255;
    }

    SDL_UpdateTexture(texture, NULL, pixels, screen_w * 4);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    Player player{screen_w / 2, screen_h - 65};
    player_x = player.x;
    player_y = player.y;
    std::vector<Missile> missiles;
    std::vector<Enemy*> enemies;

    std::chrono::time_point<std::chrono::steady_clock> begin = std::chrono::steady_clock::now();

    clock_t last_missile = 0;
    int wave = 0;
    int score = 0;
    int counter = 0;
    bool gameIsRunning = true;
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
        if (keysDown[Keys::LEFT])
        {
            player.update_pos(player.x - 3, player.y);
        }
        else if (keysDown[Keys::RIGHT])
        {
            player.update_pos(player.x + 3, player.y);
        }
        if (keysDown[Keys::UP])
        {
            if ((double)(clock() - last_missile) / CLOCKS_PER_SEC > 0.25)
            {
                missiles.push_back({player.x, player.y});
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
        
        if (counter % 200 == 0)
        {   
            std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
            int seconds = std::chrono::duration_cast<std::chrono::seconds>(now - begin).count();

            if (seconds <= 25)
            {
                wave = 0;
            }
            else if (seconds <= 30)
            {
                wave = 1;
            }
            else if (seconds <= 40)
            {
                wave = 2;
            }

            if (wave == 0)
            {
                enemies.push_back(new EnemyBasic(5, 5));
                //enemies.push_back(new Enemy3(rand() % screen_w, 5));
            }
            else if (wave == 1)
            {
                enemies.push_back(new Enemy2(rand() % screen_w, 0));
            }
            else if (wave == 2)
            {
                enemies.push_back(new Enemy3(rand() % screen_w, 5));
            }
        }

        SDL_UpdateWindowSurface(window);

        std::memset(pixels, 0, screen_w * screen_h * 4);
        move_pts(pts);

        for (const Point3D &pt : pts)
        {
            int x = static_cast<int>(pt.x);
            int y = static_cast<int>(pt.y);
            pixels[y * screen_w * 4 + x * 4 + 0] = 255;
            pixels[y * screen_w * 4 + x * 4 + 1] = 255;
            pixels[y * screen_w * 4 + x * 4 + 2] = 255;
            pixels[y * screen_w * 4 + x * 4 + 3] = 255;
        }

        SDL_UpdateTexture(texture, nullptr, pixels, screen_w * 4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        for (int i = 0; i < enemies.size(); i++)
        {
            if (!enemies[i]->is_active())
            {
                delete enemies[i];
                enemies.erase(enemies.begin() + i);
            }
            else
            {
                enemies[i]->next_pos();
            }
        }

        for (int i = 0; i < enemies.size(); i++)
        {
            for (int j = 0; j < missiles.size(); j++)
            {
                int xDisplacement = enemies[i]->get_x() - missiles[j].get_x();
                int yDisplacement = enemies[i]->get_y() - missiles[j].get_y();
                if (xDisplacement >= -30 && xDisplacement <= 10 && yDisplacement >= -30 && yDisplacement <= 0)
                {
                    score += enemies[i]->get_value();
                    delete enemies[i];
                    enemies.erase(enemies.begin() + i);
                    missiles.erase(missiles.begin() + j);
                    Mix_PlayChannel(-1, explode_sound, 0);
                }
            }
        }

        for (int i = 0; i < enemies.size(); i++)
        {   
            double distance = dist(player.x, player.y - 13, enemies[i]->get_x() + 10, enemies[i]->get_y() + 10);
            if (distance < 23) 
            {
                enemies.erase(enemies.begin() + i);
                Mix_PlayChannel(-1, explode_sound, 0);
                lives--;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (auto &missile : missiles)
        {
            missile.draw(renderer);
        }

        for (Enemy* enemy : enemies)
        {
            enemy->draw(renderer);
        }

        player.draw(renderer);

        TTF_SetFontSize(font, default_font_size);
        render_text(font, renderer, std::to_string(score).c_str(), 255, 255, 255, 5, 0);

        int w,h;
        TTF_SizeText(font, std::to_string(lives).c_str(), &w, &h);
        render_text(font, renderer, std::to_string(lives).c_str(), 255, 255, 255, screen_w - w - 5, 0);
        
        if (lives == 0)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 175);
            SDL_RenderFillRect(renderer, nullptr);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            
            const char* death_text = "you lost";
            TTF_SetFontSize(font, 100);
            TTF_SizeText(font, death_text, &w, &h);
            render_text(font, renderer, death_text, 255, 255, 255, screen_w/2 - w/2, screen_h/2 - h);
            //SDL_RenderPresent(renderer);
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
            
            const char* button_text = "Restart";
            TTF_SetFontSize(font, 60);
            TTF_SizeText(font, death_text, &w, &h);
            int button_x = (screen_w - w)/2;
            int button_y = screen_h / 2 + 30;
            SDL_Rect button_rect{button_x - 5, button_y - 5,  w + 5, h + 5};
            render_text(font, renderer, button_text, 255, 0, 0, button_x, button_y);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &button_rect);
            SDL_RenderPresent(renderer);

            bool waiting = true;
            while(waiting)
            {
                SDL_Event e;
                while(SDL_PollEvent(&e))
                {
                    switch(e.type)
                    {
                    case SDL_QUIT:
                        goto exit;
                    case SDL_MOUSEBUTTONDOWN:
                        int mouse_x, mouse_y;
                        SDL_GetMouseState(&mouse_x, &mouse_y);

                        if (mouse_x >= button_rect.x - 5 && mouse_x <= button_rect.x - 5 + button_rect.w - 5 &&
                            mouse_y >= button_rect.y - 5 && mouse_y <= button_rect.y - 5 + button_rect.h - 5)
                        {
                            waiting = false;
                        }

                        break;
                    }
                }
            }

            //reset game
            score = 0;
            lives = 3;
            begin = std::chrono::steady_clock::now();
        }

        SDL_RenderPresent(renderer);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        counter++;
    }

exit:
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();

    return 0;
}
