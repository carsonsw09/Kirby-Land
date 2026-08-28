#ifndef CAR_SCENE_HPP
#define CAR_SCENE_HPP

#include "core/Scene.hpp"
#include "graphics/TextRenderer.hpp"
#include "graphics/Texture.hpp"

#include <SDL2/SDL.h>
#include <string>

// Rhythm challenge used to start Kirby's car.
class CarScene : public Scene {
public:
    CarScene(
        SDL_Renderer* renderer,
        TextRenderer* textRenderer,
        int screenWidth,
        int screenHeight
    );

    bool load() override;
    void onEnter() override;
    void handleEvent(const SDL_Event& event) override;
    void update() override;
    void render() override;

private:
    enum class ChallengeState {
        Intro,
        Playing,
        Success
    };

    void startChallenge();
    void handleTimingPress();

    void renderBackground();
    void renderIntro();
    void renderChallenge();
    void renderTimingBar();
    void renderEngineMeter();
    void renderStatusText();

    std::string getEngineStatus() const;
    int getShakeAmount() const;

    SDL_Renderer* renderer;
    TextRenderer* textRenderer;

    int screenWidth;
    int screenHeight;

    Texture background;

    ChallengeState state;

    float enginePower;

    float indicatorPosition;
    float indicatorDirection;
    float indicatorSpeed;

    float targetStart;
    float targetEnd;

    Uint32 lastUpdateTime;

    std::string timingFeedback;

    Uint32 feedbackStartTime;
    const Uint32 feedbackDuration;

    Uint32 successStartTime;
    const Uint32 successDisplayDuration;
};

#endif