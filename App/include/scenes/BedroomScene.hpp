#ifndef BEDROOM_SCENE_HPP
#define BEDROOM_SCENE_HPP

#include "core/Scene.hpp"
#include "entities/CollectibleItem.hpp"
#include "entities/Player.hpp"
#include "graphics/TextRenderer.hpp"
#include "graphics/Texture.hpp"

#include <SDL2/SDL.h>

// Bedroom level where the player collects items before leaving.
class BedroomScene : public Scene {
public:
    BedroomScene(
        SDL_Renderer* renderer,
        TextRenderer* textRenderer,
        int screenWidth,
        int screenHeight,
        int groundY
    );

    bool load() override;
    void onEnter() override;
    void handleEvent(const SDL_Event& event) override;
    void update() override;
    void render() override;

private:
    void spawnItems();
    void checkItemCollisions();
    void renderItemCounter();
    void renderIntroMessage();
    void renderCompletionMessage();

    SDL_Renderer* renderer;
    TextRenderer* textRenderer;

    int screenWidth;
    int screenHeight;
    int groundY;

    Texture background;
    Player player;

    CollectibleItem vape;
    CollectibleItem phone;
    CollectibleItem purse;

    int collectedItems;
    const int totalItems;

    bool playerLocked;
    Uint32 roomStartTime;
    const Uint32 playerLockDuration;
};

#endif