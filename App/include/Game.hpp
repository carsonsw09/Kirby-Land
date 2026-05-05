#ifndef GAME_HPP
#define GAME_HPP

#include "Player.hpp"
#include "Texture.hpp"
#include "CollectibleItem.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

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
        Room,
        FadingToHome,
        Home
    };

    void handleEvents();
    void update();
    void render();

    void enterRoomScene();
    void enterHomeScene();

    void spawnItems();
    void checkItemCollisions();

    void renderStartScreen();
    void renderRoom();
    void renderHome();
    void renderFadeOverlay();
    void renderIntroMessage();
    void renderCompletionMessage();
    void renderMessageBox(const char* message);
    void renderItemCounter();

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* counterFont;

    bool running;

    const int screenWidth;
    const int screenHeight;
    const int groundY;

    GameState gameState;

    Texture startScreen;
    Texture background;
    Texture homeBackground;
    Player player;

    CollectibleItem vape;
    CollectibleItem phone;
    CollectibleItem purse;

    int collectedItems;
    const int totalItems;

    Uint8 fadeAlpha;
    const Uint8 fadeSpeed;

    bool playerLocked;
    Uint32 roomStartTime;
    const Uint32 playerLockDuration;
};

#endif