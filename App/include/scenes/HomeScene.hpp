#ifndef HOME_SCENE_HPP
#define HOME_SCENE_HPP

#include "core/Scene.hpp"
#include "graphics/TextRenderer.hpp"
#include "graphics/Texture.hpp"

#include <SDL2/SDL.h>

// Outside-house cutscene after the bedroom scene.
class HomeScene : public Scene {
public:
    HomeScene(
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
    SDL_Renderer* renderer;
    TextRenderer* textRenderer;

    int screenWidth;
    int screenHeight;
    int groundY;

    Texture background;
    Texture kirbyTexture;
    Texture bobTexture;

    float kirbyX;
    float kirbyY;

    int kirbyWidth;
    int kirbyHeight;

    int bobWidth;
    int bobHeight;

    int bobX;
    int bobY;

    float kirbyWalkSpeed;
    float kirbyStopX;

    bool kirbyWalking;
    bool showDialogue;
};

#endif