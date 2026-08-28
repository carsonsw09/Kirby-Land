#include "scenes/HomeScene.hpp"
#include <algorithm>

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
      kirbyWidth(340),
      kirbyHeight(300),
      bobWidth(330),
      bobHeight(300),
      bobX(0),
      bobY(0),
      kirbyWalkSpeed(2.6f),
      kirbyStopX(0.0f),
      kirbyWalking(true),
      showDialogue(false),
      leavingScene(false),
      fadingOut(false),
      fadeAlpha(0),
      currentDialogueIndex(0){
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

    bobWidth = 330;
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
    currentDialogueIndex = 0;

    leavingScene = false;
    fadingOut = false;
    fadeAlpha = 0;

    setupDialogue();
}

void HomeScene::setupDialogue() {
    dialogueLines.clear();

    dialogueLines.push_back({
        "Bob",
        "What are you doing right now, don't you have work!!!?"
    });

    dialogueLines.push_back({
        "Kirby",
        "Sorry I was smooching on my big prickly iguana"
    });

    dialogueLines.push_back({
        "Bob",
        "EWWWW Kenzie I don't ever want to hear that again"
    });
}

void HomeScene::handleEvent(const SDL_Event& event) {
    if (!showDialogue) {
        return;
    }

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        if (event.key.keysym.sym == SDLK_SPACE) {
            advanceDialogue();
        }
    }
}

void HomeScene::advanceDialogue() {
    if (currentDialogueIndex <
        static_cast<int>(dialogueLines.size()) - 1) {

        currentDialogueIndex++;
    }
    else {
        // Dialogue is finished.
        showDialogue = false;
        leavingScene = true;
    }
}

void HomeScene::update() {

    // Kirby's initial walk toward Bob.
    if (kirbyWalking) {
        kirbyX += kirbyWalkSpeed;

        if (kirbyX >= kirbyStopX) {
            kirbyX = kirbyStopX;
            kirbyWalking = false;
            showDialogue = true;
            currentDialogueIndex = 0;
        }

        return;
    }

    // Kirby leaves toward the car.
    if (leavingScene && !fadingOut) {

        kirbyX -= kirbyWalkSpeed;

        // Stop point near the car.
        int carStopX = 300;

        if (kirbyX <= carStopX) {
            kirbyX = static_cast<float>(carStopX);
            fadingOut = true;
        }

        return;
    }

    // Slowly fade the entire scene to black.
    if (fadingOut) {

    if (fadeAlpha < 255) {
        fadeAlpha = static_cast<Uint8>(
            std::min(255, static_cast<int>(fadeAlpha) + 2)
        );
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

    // This matches the existing direction fix for Kirby facing right.
    kirbyTexture.renderFlipped(
        renderer,
        static_cast<int>(kirbyX),
        static_cast<int>(kirbyY),
        kirbyWidth,
        kirbyHeight,
        SDL_FLIP_HORIZONTAL
    );

    if (showDialogue) {
        renderCurrentDialogue();
    }
    if (fadingOut) {
    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        fadeAlpha
    );

    SDL_Rect fadeRect = {
        0,
        0,
        screenWidth,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &fadeRect
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );
}
}

void HomeScene::renderCurrentDialogue() {
    if (dialogueLines.empty()) {
        return;
    }

    const DialogueLine& currentLine =
        dialogueLines[currentDialogueIndex];

    int dialogueCenterX;
    int dialogueY;

    if (currentLine.speaker == "Bob") {
        dialogueCenterX = bobX + bobWidth / 2;

        // Higher above Bob.
        dialogueY = bobY - 190;

        textRenderer->renderSpeechBubble(
            renderer,
            currentLine.text,
            dialogueCenterX,
            dialogueY,
            500,
            false
        );
    }
    else {
        dialogueCenterX =
            static_cast<int>(kirbyX) + kirbyWidth / 2;

        // Higher above Kirby.
        dialogueY =
            static_cast<int>(kirbyY) - 190;

        textRenderer->renderSpeechBubble(
            renderer,
            currentLine.text,
            dialogueCenterX,
            dialogueY,
            500,
            true
        );
    }
}

    