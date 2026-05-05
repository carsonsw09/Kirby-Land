#ifndef GAME_HPP
#define GAME_HPP

#include "Player.hpp"
#include "Texture.hpp"

#include <SDL2/SDL.h>

// Controls the main game setup, screens, updates, rendering, and cleanup.
class Game {
public:
    Game();
    ~Game();

    bool initialize();
    void run();
    void shutdown();

private:
    enum class GameState {
        StartScreen,
        FadingToRoom,
        Room
    };

    void handleEvents();
    void update();
    void render();

    void renderStartScreen();
    void renderRoom();
    void renderFadeOverlay();

    SDL_Window* window;
    SDL_Renderer* renderer;

    bool running;

    const int screenWidth;
    const int screenHeight;
    const int groundY;

    GameState gameState;

    Texture startScreen;
    Texture background;
    Player player;

    Uint8 fadeAlpha;
    const Uint8 fadeSpeed;
};

#endif