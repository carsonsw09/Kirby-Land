#include "core/Game.hpp"

#include "scenes/BedroomScene.hpp"
#include "scenes/HomeScene.hpp"
#include "scenes/StartScene.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

Game::Game()
    : Game(SceneType::Start) {
}

Game::Game(SceneType startingScene)
    : window(nullptr),
      renderer(nullptr),
      currentScene(nullptr),
      running(false),
      screenWidth(1280),
      screenHeight(720),
      groundY(650),
      startingScene(startingScene),
      transitionState(TransitionState::None),
      pendingScene(SceneType::None),
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

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize.\n";
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

    if (!textRenderer.loadFonts()) {
        return false;
    }

    if (!changeScene(startingScene)) {
    return false;
    }

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

        if (currentScene != nullptr && transitionState == TransitionState::None) {
            currentScene->handleEvent(event);
        }
    }
}

void Game::update() {
    if (transitionState == TransitionState::FadingOut) {
        if (fadeAlpha + fadeSpeed >= 255) {
            fadeAlpha = 255;

            if (!changeScene(pendingScene)) {
                running = false;
                return;
            }

            transitionState = TransitionState::FadingIn;
        }
        else {
            fadeAlpha += fadeSpeed;
        }

        return;
    }

    if (transitionState == TransitionState::FadingIn) {
        if (fadeAlpha <= fadeSpeed) {
            fadeAlpha = 0;
            transitionState = TransitionState::None;
            pendingScene = SceneType::None;
        }
        else {
            fadeAlpha -= fadeSpeed;
        }

        return;
    }

    if (currentScene != nullptr) {
        currentScene->update();

        SceneType requestedScene = currentScene->getRequestedScene();

        if (requestedScene != SceneType::None) {
            currentScene->clearRequestedScene();
            requestSceneChange(requestedScene);
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (currentScene != nullptr) {
        currentScene->render();
    }

    if (transitionState != TransitionState::None) {
        renderFadeOverlay();
    }

    SDL_RenderPresent(renderer);
}

bool Game::requestSceneChange(SceneType sceneType) {
    if (sceneType == SceneType::None) {
        return false;
    }

    pendingScene = sceneType;
    transitionState = TransitionState::FadingOut;
    fadeAlpha = 0;

    return true;
}

bool Game::changeScene(SceneType sceneType) {
    switch (sceneType) {
        case SceneType::Start:
            currentScene = std::make_unique<StartScene>(
                renderer,
                screenWidth,
                screenHeight
            );
            break;

        case SceneType::Bedroom:
            currentScene = std::make_unique<BedroomScene>(
                renderer,
                &textRenderer,
                screenWidth,
                screenHeight,
                groundY
            );
            break;

        case SceneType::Home:
            currentScene = std::make_unique<HomeScene>(
                renderer,
                &textRenderer,
                screenWidth,
                screenHeight,
                groundY
            );
            break;

    break;

        case SceneType::None:
        default:
            return false;
    }

    if (!currentScene->load()) {
        return false;
    }

    currentScene->onEnter();

    return true;
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
    currentScene.reset();

    textRenderer.shutdown();

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