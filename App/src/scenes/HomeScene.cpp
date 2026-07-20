#include "scenes/HomeScene.hpp"

HomeScene::HomeScene(
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
      kirbyX(0.0f),
      kirbyY(0.0f),
      kirbyWidth(300),
      kirbyHeight(300),
      bobWidth(240),
      bobHeight(300),
      bobX(0),
      bobY(0),
      kirbyWalkSpeed(2.6f),
      kirbyStopX(0.0f),
      kirbyWalking(true),
      showDialogue(false) {
}

bool HomeScene::load() {
    if (!background.loadFromFile(renderer, "assets/images/backgrounds/kirby_home.png")) {
        return false;
    }

    if (!kirbyTexture.loadFromFile(renderer, "assets/images/kirby_player/kirby_golf.png")) {
        return false;
    }

    if (!bobTexture.loadFromFile(renderer, "assets/images/other_characters/angry_bob.png")) {
        return false;
    }

    return true;
}

void HomeScene::onEnter() {
    clearRequestedScene();

    kirbyWidth = 340;
    kirbyHeight = 300;

    bobWidth = 280;
    bobHeight = 300;

    kirbyX = static_cast<float>(-kirbyWidth);
    kirbyY = static_cast<float>(groundY - kirbyHeight);

// Kirby stops roughly halfway through the screen.
    kirbyStopX = static_cast<float>((screenWidth / 2) - (kirbyWidth / 2));

// Bob stands a bit right of center, near the front door.
    bobX = (screenWidth / 2) + 210;
    bobY = groundY - bobHeight;

    kirbyWalking = true;
    showDialogue = false;
}

void HomeScene::handleEvent(const SDL_Event& event) {
    (void)event;
}

void HomeScene::update() {
    if (kirbyWalking) {
        kirbyX += kirbyWalkSpeed;

        if (kirbyX >= kirbyStopX) {
            kirbyX = kirbyStopX;
            kirbyWalking = false;
            showDialogue = true;
        }
    }
}

void HomeScene::render() {
    background.render(renderer, 0, 0, screenWidth, screenHeight);

    // Bob is drawn first so Kirby can appear slightly in front if they overlap.
    bobTexture.render(
    renderer,
    bobX,
    bobY,
    bobWidth,
    bobHeight
);

    // This matches your existing direction fix for Kirby facing right.
    kirbyTexture.renderFlipped(
    renderer,
    static_cast<int>(kirbyX),
    static_cast<int>(kirbyY),
    kirbyWidth,
    kirbyHeight,
    SDL_FLIP_HORIZONTAL
);

    if (showDialogue) {
    int bobCenterX = bobX + bobWidth / 2;
    int dialogueY = bobY - 200;

    textRenderer->renderDialogueBox(
        renderer,
        "What are you doing right now, don't you have work!!!?",
        bobCenterX,
        dialogueY,
        500
    );
}
}