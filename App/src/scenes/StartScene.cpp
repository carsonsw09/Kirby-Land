#include "scenes/StartScene.hpp"

StartScene::StartScene(SDL_Renderer* renderer, int screenWidth, int screenHeight)
    : renderer(renderer),
      screenWidth(screenWidth),
      screenHeight(screenHeight) {
}

bool StartScene::load() {
    return background.loadFromFile(
        renderer,
        "assets/images/backgrounds/first_screen.png"
    );
}

void StartScene::onEnter() {
    clearRequestedScene();
}

void StartScene::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            requestedScene = SceneType::Bedroom;
        }
    }
}

void StartScene::update() {
}

void StartScene::render() {
    background.render(renderer, 0, 0, screenWidth, screenHeight);
}