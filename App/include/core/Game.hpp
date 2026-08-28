#ifndef GAME_HPP
#define GAME_HPP

#include "core/Scene.hpp"
#include "core/SceneType.hpp"
#include "graphics/TextRenderer.hpp"

#include <SDL2/SDL.h>
#include <memory>

// Main application controller.
class Game {
public:
    Game();
    explicit Game(SceneType startingScene);
    ~Game();

    bool initialize();
    void run();
    void shutdown();

private:
    enum class TransitionState {
        None,
        FadingOut,
        FadingIn
    };

    void handleEvents();
    void update();
    void render();

    bool changeScene(SceneType sceneType);
    bool requestSceneChange(SceneType sceneType);

    void renderFadeOverlay();

    SDL_Window* window;
    SDL_Renderer* renderer;

    TextRenderer textRenderer;

    std::unique_ptr<Scene> currentScene;

    bool running;

    const int screenWidth;
    const int screenHeight;
    const int groundY;

    SceneType startingScene;

    TransitionState transitionState;
    SceneType pendingScene;

    Uint8 fadeAlpha;
    const Uint8 fadeSpeed;
};

#endif