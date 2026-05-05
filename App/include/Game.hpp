#ifndef GAME_HPP
#define GAME_HPP

#include "Player.hpp"
#include "Texture.hpp"

#include <SDL2/SDL.h>

// Runs the main game setup, loop, updates, rendering, and cleanup.
class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void shutdown();

private:
    void handleEvents();
    void update();
    void render();

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool running;

    const int screenWidth;
    const int screenHeight;
    const int groundY;

    Texture background;
    Player player;
};

#endif