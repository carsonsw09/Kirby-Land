#include "Game.hpp"

#include <SDL2/SDL_image.h>
#include <iostream>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      running(false),
      screenWidth(1280),
      screenHeight(720),
      groundY(650),
      gameState(GameState::StartScreen),
      fadeAlpha(0),
      fadeSpeed(4) {
}

Game::~Game() {
    shutdown();
}

bool Game::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    int imageFlags = IMG_INIT_PNG;

    if ((IMG_Init(imageFlags) & imageFlags) != imageFlags) {
        std::cerr << "SDL_image could not initialize PNG support.\n";
        std::cerr << "IMG Error: " << IMG_GetError() << "\n";
        return false;
    }

    window = SDL_CreateWindow(
        "Kirby Land",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,
        SDL_WINDOW_SHOWN
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    if (!startScreen.loadFromFile(renderer, "assets/images/backgrounds/first_screen.png")) {
        return false;
    }

    if (!background.loadFromFile(renderer, "assets/images/backgrounds/kirby_background.png")) {
        return false;
    }

    if (!player.loadTexture(renderer, "assets/images/kirby_player/kirby_pajamas.png")) {
        return false;
    }

    player.setPosition(
        100.0f,
        static_cast<float>(groundY - player.getHeight())
    );

    running = true;
    return true;
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }

        if (gameState == GameState::StartScreen) {
            if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    gameState = GameState::FadingToRoom;
                    fadeAlpha = 0;
                }
            }
        }
        else if (gameState == GameState::Room) {
            player.handleInput(event);
        }
    }
}

void Game::update() {
    if (gameState == GameState::FadingToRoom) {
        if (fadeAlpha + fadeSpeed >= 255) {
            fadeAlpha = 255;
            gameState = GameState::Room;
        }
        else {
            fadeAlpha += fadeSpeed;
        }
    }

    if (gameState == GameState::Room) {
        player.update(screenWidth, groundY);
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameState == GameState::StartScreen) {
        renderStartScreen();
    }
    else if (gameState == GameState::FadingToRoom) {
        renderStartScreen();
        renderFadeOverlay();
    }
    else if (gameState == GameState::Room) {
        renderRoom();
    }

    SDL_RenderPresent(renderer);
}

void Game::renderStartScreen() {
    startScreen.render(renderer, 0, 0, screenWidth, screenHeight);
}

void Game::renderRoom() {
    background.render(renderer, 0, 0, screenWidth, screenHeight);
    player.render(renderer);
}

void Game::renderFadeOverlay() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, fadeAlpha);

    SDL_Rect fadeRect = {
        0,
        0,
        screenWidth,
        screenHeight
    };

    SDL_RenderFillRect(renderer, &fadeRect);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Game::shutdown() {
    startScreen.free();
    background.free();

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    IMG_Quit();
    SDL_Quit();
}