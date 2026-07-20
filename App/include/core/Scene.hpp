#ifndef SCENE_HPP
#define SCENE_HPP

#include "core/SceneType.hpp"

#include <SDL2/SDL.h>

// Base class for all game scenes.
class Scene {
public:
    virtual ~Scene() = default;

    virtual bool load() = 0;
    virtual void onEnter() = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
    virtual void update() = 0;
    virtual void render() = 0;

    SceneType getRequestedScene() const {
        return requestedScene;
    }

    void clearRequestedScene() {
        requestedScene = SceneType::None;
    }

protected:
    SceneType requestedScene = SceneType::None;
};

#endif