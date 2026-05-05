#include "Game.hpp"

#include <SDL2/SDL_image.h>
#include <iostream>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      running(false),
      screenWidth(1280),
      screenHeight(720),
      groundY(650) {
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

        player.handleInput(event);
    }
}

void Game::update() {
    player.update(screenWidth, groundY);
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    background.render(renderer, 0, 0, screenWidth, screenHeight);
    player.render(renderer);

    SDL_RenderPresent(renderer);
}

void Game::shutdown() {
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