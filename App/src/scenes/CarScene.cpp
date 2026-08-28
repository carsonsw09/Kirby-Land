#include "scenes/CarScene.hpp"

#include <cmath>

CarScene::CarScene(
    SDL_Renderer* renderer,
    TextRenderer* textRenderer,
    int screenWidth,
    int screenHeight
)
    : renderer(renderer),
      textRenderer(textRenderer),
      screenWidth(screenWidth),
      screenHeight(screenHeight),
      state(ChallengeState::Intro),
      enginePower(0.0f),
      indicatorPosition(0.0f),
      indicatorDirection(1.0f),
      indicatorSpeed(0.85f),
      targetStart(0.42f),
      targetEnd(0.58f),
      lastUpdateTime(0),
      timingFeedback(""),
      feedbackStartTime(0),
      feedbackDuration(450),
      successStartTime(0),
      successDisplayDuration(1500) {
}

bool CarScene::load() {
    return background.loadFromFile(
        renderer,
        "assets/images/backgrounds/car_scene.png"
    );
}

void CarScene::onEnter() {
    clearRequestedScene();

    state = ChallengeState::Intro;

    enginePower = 0.0f;

    indicatorPosition = 0.0f;
    indicatorDirection = 1.0f;

    timingFeedback.clear();

    feedbackStartTime = 0;
    successStartTime = 0;

    lastUpdateTime = SDL_GetTicks();
}

void CarScene::handleEvent(const SDL_Event& event) {
    if (
        event.type != SDL_KEYDOWN ||
        event.key.repeat != 0 ||
        event.key.keysym.sym != SDLK_SPACE
    ) {
        return;
    }

    if (state == ChallengeState::Intro) {
        startChallenge();
        return;
    }

    if (state == ChallengeState::Playing) {
        handleTimingPress();
    }
}

void CarScene::startChallenge() {
    state = ChallengeState::Playing;

    enginePower = 0.0f;

    indicatorPosition = 0.0f;
    indicatorDirection = 1.0f;

    timingFeedback.clear();

    lastUpdateTime = SDL_GetTicks();
}

void CarScene::handleTimingPress() {
    bool correctTiming =
        indicatorPosition >= targetStart &&
        indicatorPosition <= targetEnd;

    feedbackStartTime = SDL_GetTicks();

    if (correctTiming) {
        timingFeedback = "GOOD!";

        enginePower += 10.0f;

        if (enginePower >= 100.0f) {
            enginePower = 100.0f;

            state = ChallengeState::Success;
            successStartTime = SDL_GetTicks();

            timingFeedback = "VROOOM!";
        }
    }
    else {
        timingFeedback = "MISS!";
    }
}

void CarScene::update() {
    Uint32 currentTime = SDL_GetTicks();

    float deltaTime =
        static_cast<float>(currentTime - lastUpdateTime) /
        1000.0f;

    lastUpdateTime = currentTime;

    if (state == ChallengeState::Playing) {
        indicatorPosition +=
            indicatorDirection *
            indicatorSpeed *
            deltaTime;

        if (indicatorPosition >= 1.0f) {
            indicatorPosition = 1.0f;
            indicatorDirection = -1.0f;
        }

        if (indicatorPosition <= 0.0f) {
            indicatorPosition = 0.0f;
            indicatorDirection = 1.0f;
        }
    }

    if (
        !timingFeedback.empty() &&
        currentTime - feedbackStartTime >= feedbackDuration &&
        state != ChallengeState::Success
    ) {
        timingFeedback.clear();
    }

    // Once the engine has started, show the success message
    // briefly before moving into the driving scene.
    if (state == ChallengeState::Success) {
        if (
            currentTime - successStartTime >=
            successDisplayDuration
        ) {
            requestedScene = SceneType::Driving;
        }
    }
}

void CarScene::render() {
    renderBackground();

    if (state == ChallengeState::Intro) {
        renderIntro();
    }
    else {
        renderChallenge();
    }
}

void CarScene::renderBackground() {
    int shakeAmount = getShakeAmount();

    int shakeX = 0;
    int shakeY = 0;

    if (shakeAmount > 0) {
        Uint32 currentTime = SDL_GetTicks();

        shakeX = static_cast<int>(
            std::sin(currentTime * 0.055) *
            shakeAmount
        );

        shakeY = static_cast<int>(
            std::cos(currentTime * 0.073) *
            shakeAmount
        );
    }

    const int padding = 12;

    background.renderCover(
        renderer,
        -padding + shakeX,
        -padding + shakeY,
        screenWidth + padding * 2,
        screenHeight + padding * 2
    );
}

void CarScene::renderIntro() {
    textRenderer->renderMessageBox(
        renderer,
        "Hit the SPACE bar at the right time and start the car!",
        screenWidth,
        45
    );

    textRenderer->renderFloatingText(
        renderer,
        "Press SPACE to begin",
        screenWidth / 2,
        screenHeight - 115,
        500
    );
}

void CarScene::renderChallenge() {
    renderTimingBar();
    renderEngineMeter();
    renderStatusText();
}

void CarScene::renderTimingBar() {
    const int barWidth = 620;
    const int barHeight = 34;

    const int barX =
        (screenWidth - barWidth) / 2;

    const int barY = 80;

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        190
    );

    SDL_Rect outerBox = {
        barX - 16,
        barY - 45,
        barWidth + 32,
        barHeight + 80
    };

    SDL_RenderFillRect(
        renderer,
        &outerBox
    );

    SDL_SetRenderDrawColor(
        renderer,
        225,
        225,
        225,
        255
    );

    SDL_Rect barRect = {
        barX,
        barY,
        barWidth,
        barHeight
    };

    SDL_RenderFillRect(
        renderer,
        &barRect
    );

    int targetX =
        barX +
        static_cast<int>(
            targetStart * barWidth
        );

    int targetWidth =
        static_cast<int>(
            (targetEnd - targetStart) *
            barWidth
        );

    SDL_SetRenderDrawColor(
        renderer,
        90,
        190,
        110,
        255
    );

    SDL_Rect targetRect = {
        targetX,
        barY,
        targetWidth,
        barHeight
    };

    SDL_RenderFillRect(
        renderer,
        &targetRect
    );

    int markerX =
        barX +
        static_cast<int>(
            indicatorPosition * barWidth
        );

    SDL_SetRenderDrawColor(
        renderer,
        220,
        60,
        60,
        255
    );

    SDL_Rect markerRect = {
        markerX - 5,
        barY - 8,
        10,
        barHeight + 16
    };

    SDL_RenderFillRect(
        renderer,
        &markerRect
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );

    textRenderer->renderFloatingText(
        renderer,
        "PRESS SPACE IN THE GREEN ZONE",
        screenWidth / 2,
        barY - 38,
        600
    );
}

void CarScene::renderEngineMeter() {
    const int meterWidth = 500;
    const int meterHeight = 32;

    int meterX =
        (screenWidth - meterWidth) / 2;

    int meterY =
        screenHeight - 145;

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        200
    );

    SDL_Rect meterBackground = {
        meterX - 15,
        meterY - 50,
        meterWidth + 30,
        meterHeight + 80
    };

    SDL_RenderFillRect(
        renderer,
        &meterBackground
    );

    SDL_SetRenderDrawColor(
        renderer,
        70,
        70,
        70,
        255
    );

    SDL_Rect emptyMeter = {
        meterX,
        meterY,
        meterWidth,
        meterHeight
    };

    SDL_RenderFillRect(
        renderer,
        &emptyMeter
    );

    int filledWidth =
        static_cast<int>(
            meterWidth *
            (enginePower / 100.0f)
        );

    SDL_SetRenderDrawColor(
        renderer,
        230,
        170,
        55,
        255
    );

    SDL_Rect filledMeter = {
        meterX,
        meterY,
        filledWidth,
        meterHeight
    };

    SDL_RenderFillRect(
        renderer,
        &filledMeter
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );

    textRenderer->renderFloatingText(
        renderer,
        "ENGINE POWER",
        screenWidth / 2,
        meterY - 43,
        400
    );
}

void CarScene::renderStatusText() {
    std::string status = getEngineStatus();

    textRenderer->renderFloatingText(
        renderer,
        status,
        screenWidth / 2,
        screenHeight - 65,
        700
    );

    if (!timingFeedback.empty()) {
        textRenderer->renderFloatingText(
            renderer,
            timingFeedback,
            screenWidth / 2,
            165,
            400
        );
    }
}

std::string CarScene::getEngineStatus() const {
    if (enginePower >= 100.0f) {
        return "VROOOM! ENGINE STARTED!";
    }

    if (enginePower >= 75.0f) {
        return "ALMOST THERE...";
    }

    if (enginePower >= 50.0f) {
        return "ENGINE TURNING OVER...";
    }

    if (enginePower >= 25.0f) {
        return "ENGINE SPUTTERING...";
    }

    return "CLICK... CLICK...";
}

int CarScene::getShakeAmount() const {
    if (state == ChallengeState::Intro) {
        return 0;
    }

    if (enginePower >= 100.0f) {
        return 9;
    }

    if (enginePower >= 75.0f) {
        return 6;
    }

    if (enginePower >= 50.0f) {
        return 4;
    }

    if (enginePower >= 25.0f) {
        return 2;
    }

    return 0;
}