#include "Game.hpp"

#include <SDL2/SDL_image.h>
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

Game::Game()
    : window(nullptr),
      renderer(nullptr),
      font(nullptr),
      counterFont(nullptr),
      running(false),
      screenWidth(1280),
      screenHeight(720),
      groundY(650),
      gameState(GameState::StartScreen),
      collectedItems(0),
      totalItems(3),
      fadeAlpha(0),
      fadeSpeed(4),
      playerLocked(true),
      roomStartTime(0),
      playerLockDuration(7000) {
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

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return false;
    }

    font = TTF_OpenFont(
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf",
        30
    );

    if (font == nullptr) {
        font = TTF_OpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf",
            30
        );
    }

    if (font == nullptr) {
        font = TTF_OpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            30
        );
    }

    if (font == nullptr) {
        font = TTF_OpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            30
        );
    }

    if (font == nullptr) {
        std::cerr << "Could not load intro font.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return false;
    }

    counterFont = TTF_OpenFont(
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf",
        44
    );

    if (counterFont == nullptr) {
        counterFont = TTF_OpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            44
        );
    }

    if (counterFont == nullptr) {
        std::cerr << "Could not load counter font.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
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

    if (!homeBackground.loadFromFile(renderer, "assets/images/backgrounds/kirby_home.png")) {
        return false;
    }

    if (!player.loadTexture(renderer, "assets/images/kirby_player/kirby_pajamas.png")) {
        return false;
    }

    if (!vape.loadTexture(renderer, "assets/images/items/vape.png")) {
        return false;
    }

    if (!phone.loadTexture(renderer, "assets/images/items/phone.png")) {
        return false;
    }

    if (!purse.loadTexture(renderer, "assets/images/items/purse.png")) {
        return false;
    }

    vape.setSize(45, 55);
    phone.setSize(55, 65);
    purse.setSize(95, 85);

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
            if (!playerLocked) {
                player.handleInput(event);
            }
        }
    }
}

void Game::update() {
    if (gameState == GameState::FadingToRoom) {
        if (fadeAlpha + fadeSpeed >= 255) {
            fadeAlpha = 255;
            enterRoomScene();
        }
        else {
            fadeAlpha += fadeSpeed;
        }
    }

    if (gameState == GameState::FadingToHome) {
        if (fadeAlpha + fadeSpeed >= 255) {
            fadeAlpha = 255;
            enterHomeScene();
        }
        else {
            fadeAlpha += fadeSpeed;
        }
    }

    if (gameState == GameState::Room) {
        Uint32 currentTime = SDL_GetTicks();

        if (playerLocked && currentTime - roomStartTime >= playerLockDuration) {
            playerLocked = false;
        }

        if (!playerLocked) {
            player.update(screenWidth, groundY);
            checkItemCollisions();

            if (collectedItems == totalItems && player.isAtRightEdge(screenWidth)) {
                gameState = GameState::FadingToHome;
                fadeAlpha = 0;
            }
        }
    }
}

void Game::enterRoomScene() {
    gameState = GameState::Room;
    playerLocked = true;
    roomStartTime = SDL_GetTicks();

    collectedItems = 0;

    vape.reset();
    phone.reset();
    purse.reset();

    float centeredX = static_cast<float>((screenWidth - player.getWidth()) / 2);
    float groundedY = static_cast<float>(groundY - player.getHeight());

    player.setPosition(centeredX, groundedY);

    spawnItems();
}

void Game::enterHomeScene() {
    gameState = GameState::Home;
}

void Game::spawnItems() {
    // These points intentionally avoid the center of the room,
    // where Kirby starts locked in place.
    std::vector<SDL_Point> spawnPoints = {
        {125, 565},
        {230, 510},
        {350, 575},
        {885, 560},
        {990, 520},
        {1115, 585},
        {1180, 500}
    };

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());

    std::shuffle(spawnPoints.begin(), spawnPoints.end(), generator);

    vape.setPosition(spawnPoints[0].x, spawnPoints[0].y);
    phone.setPosition(spawnPoints[1].x, spawnPoints[1].y);
    purse.setPosition(spawnPoints[2].x, spawnPoints[2].y);
}

void Game::checkItemCollisions() {
    SDL_Rect playerBox = player.getCollisionBox();

    if (!vape.isCollected()) {
        SDL_Rect vapeBox = vape.getCollisionBox();

        if (SDL_HasIntersection(&playerBox, &vapeBox)) {
            vape.collect();
            collectedItems++;
        }
    }

    if (!phone.isCollected()) {
        SDL_Rect phoneBox = phone.getCollisionBox();

        if (SDL_HasIntersection(&playerBox, &phoneBox)) {
            phone.collect();
            collectedItems++;
        }
    }

    if (!purse.isCollected()) {
        SDL_Rect purseBox = purse.getCollisionBox();

        if (SDL_HasIntersection(&playerBox, &purseBox)) {
            purse.collect();
            collectedItems++;
        }
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
        renderItemCounter();

        if (playerLocked) {
            renderIntroMessage();
        }
        else if (collectedItems == totalItems) {
            renderCompletionMessage();
        }
    }
    else if (gameState == GameState::FadingToHome) {
        renderRoom();
        renderItemCounter();
        renderCompletionMessage();
        renderFadeOverlay();
    }
    else if (gameState == GameState::Home) {
        renderHome();
    }

    SDL_RenderPresent(renderer);
}

void Game::renderStartScreen() {
    startScreen.render(renderer, 0, 0, screenWidth, screenHeight);
}

void Game::renderRoom() {
    background.render(renderer, 0, 0, screenWidth, screenHeight);

    vape.render(renderer);
    phone.render(renderer);
    purse.render(renderer);

    player.render(renderer);
}

void Game::renderHome() {
    homeBackground.render(renderer, 0, 0, screenWidth, screenHeight);
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

void Game::renderIntroMessage() {
    renderMessageBox(
        "You woke up late and have work in 30 mins, find your vape, bag, and phone before you can go."
    );
}

void Game::renderCompletionMessage() {
    renderMessageBox(
        "Well done now hurry up to work Coleens on the clock."
    );
}

void Game::renderMessageBox(const char* message) {
    if (font == nullptr) {
        return;
    }

    SDL_Color textColor = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(
        font,
        message,
        textColor,
        screenWidth - 80
    );

    if (textSurface == nullptr) {
        std::cerr << "Could not create text surface.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == nullptr) {
        std::cerr << "Could not create text texture.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {
        (screenWidth - textWidth) / 2,
        25,
        textWidth,
        textHeight
    };

    SDL_Rect boxRect = {
        textRect.x - 20,
        textRect.y - 12,
        textRect.w + 40,
        textRect.h + 24
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 130);
    SDL_RenderFillRect(renderer, &boxRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Game::renderItemCounter() {
    if (counterFont == nullptr) {
        return;
    }

    std::string counterText =
        std::to_string(collectedItems) + "/" + std::to_string(totalItems);

    SDL_Color textColor = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* textSurface = TTF_RenderText_Blended(
        counterFont,
        counterText.c_str(),
        textColor
    );

    if (textSurface == nullptr) {
        std::cerr << "Could not create counter surface.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == nullptr) {
        std::cerr << "Could not create counter texture.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {
        screenWidth - textWidth - 35,
        25,
        textWidth,
        textHeight
    };

    SDL_Rect boxRect = {
        textRect.x - 16,
        textRect.y - 10,
        textRect.w + 32,
        textRect.h + 20
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, &boxRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}

void Game::shutdown() {
    startScreen.free();
    background.free();
    homeBackground.free();

    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    if (counterFont != nullptr) {
        TTF_CloseFont(counterFont);
        counterFont = nullptr;
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}