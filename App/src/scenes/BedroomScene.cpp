#include "scenes/BedroomScene.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

BedroomScene::BedroomScene(
    SDL_Renderer* renderer,
    TextRenderer* textRenderer,
    int screenWidth,
    int screenHeight,
    int groundY
)
    : renderer(renderer),
      textRenderer(textRenderer),
      screenWidth(screenWidth),
      screenHeight(screenHeight),
      groundY(groundY),
      collectedItems(0),
      totalItems(3),
      playerLocked(true),
      roomStartTime(0),
      playerLockDuration(7000) {
}

bool BedroomScene::load() {
    if (!background.loadFromFile(renderer, "assets/images/backgrounds/kirby_background.png")) {
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

    return true;
}

void BedroomScene::onEnter() {
    clearRequestedScene();

    collectedItems = 0;

    vape.reset();
    phone.reset();
    purse.reset();

    playerLocked = true;
    roomStartTime = SDL_GetTicks();

    float centeredX = static_cast<float>((screenWidth - player.getWidth()) / 2);
    float groundedY = static_cast<float>(groundY - player.getHeight());

    player.setPosition(centeredX, groundedY);

    spawnItems();
}

void BedroomScene::handleEvent(const SDL_Event& event) {
    if (!playerLocked) {
        player.handleInput(event);
    }
}

void BedroomScene::update() {
    Uint32 currentTime = SDL_GetTicks();

    if (playerLocked && currentTime - roomStartTime >= playerLockDuration) {
        playerLocked = false;
    }

    if (!playerLocked) {
        player.update(screenWidth, groundY);
        checkItemCollisions();

        if (collectedItems == totalItems && player.isAtRightEdge(screenWidth)) {
            requestedScene = SceneType::Home;
        }
    }
}

void BedroomScene::render() {
    background.render(renderer, 0, 0, screenWidth, screenHeight);

    vape.render(renderer);
    phone.render(renderer);
    purse.render(renderer);

    player.render(renderer);

    renderItemCounter();

    if (playerLocked) {
        renderIntroMessage();
    }
    else if (collectedItems == totalItems) {
        renderCompletionMessage();
    }
}

void BedroomScene::spawnItems() {
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

void BedroomScene::checkItemCollisions() {
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

void BedroomScene::renderItemCounter() {
    std::string counterText =
        std::to_string(collectedItems) + "/" + std::to_string(totalItems);

    textRenderer->renderCounter(
        renderer,
        counterText,
        screenWidth,
        25
    );
}

void BedroomScene::renderIntroMessage() {
    textRenderer->renderMessageBox(
        renderer,
        "You woke up late and have work in 30 mins, find your vape, bag, and phone before you can go.",
        screenWidth,
        25
    );
}

void BedroomScene::renderCompletionMessage() {
    textRenderer->renderMessageBox(
        renderer,
        "Well done now hurry up to work Coleens on the clock.",
        screenWidth,
        25
    );
}