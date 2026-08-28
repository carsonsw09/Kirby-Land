#ifndef START_SCENE_HPP
#define START_SCENE_HPP

#include "core/Scene.hpp"
#include "graphics/Texture.hpp"

#include <SDL2/SDL.h>

// First screen shown before the bedroom scene.
class StartScene : public Scene {
public:
    StartScene(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    bool load() override;
    void onEnter() override;
    void handleEvent(const SDL_Event& event) override;
    void update() override;
    void render() override;

private:
    SDL_Renderer* renderer;

    int screenWidth;
    int screenHeight;

    Texture background;
};

#endif