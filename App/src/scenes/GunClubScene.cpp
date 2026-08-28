#include "scenes/GunClubScene.hpp"

#include <algorithm>
#include <array>
#include <cmath>

GunClubScene::GunClubScene(
    SDL_Renderer* renderer,
    TextRenderer* textRenderer,
    int screenWidth,
    int screenHeight
)
    : renderer(renderer),
      textRenderer(textRenderer),
      screenWidth(screenWidth),
      screenHeight(screenHeight),
      roadWidth(620),
      roadX((screenWidth - roadWidth) / 2),
      playerX(0.0f),
      playerY(0.0f),
      playerWidth(70),
      playerHeight(120),
      playerSpeed(390.0f),
      movingLeft(false),
      movingRight(false),
      roadScrollOffset(0.0f),
      damageScrollOffset(0.0f),
      roadSpeed(310.0f),
      sceneStartTime(0),
      lastUpdateTime(0),
      lastSpawnTime(0),
      levelDuration(30000),
      levelComplete(false),
      hits(0),
      hitMessageStart(0),
      hitMessageDuration(550),
      randomGenerator(std::random_device{}()) {
}

bool GunClubScene::load() {
    return true;
}

void GunClubScene::onEnter() {
    clearRequestedScene();

    roadX =
        (screenWidth -
         roadWidth) / 2;

    playerX =
        static_cast<float>(
            (screenWidth -
             playerWidth) / 2
        );

    playerY =
        static_cast<float>(
            screenHeight -
            playerHeight -
            45
        );

    movingLeft = false;
    movingRight = false;

    roadScrollOffset = 0.0f;
    damageScrollOffset = 0.0f;

    hazards.clear();
    potholes.clear();

    createPotholes();

    sceneStartTime =
        SDL_GetTicks();

    lastUpdateTime =
        sceneStartTime;

    lastSpawnTime =
        sceneStartTime;

    levelComplete = false;

    hits = 0;

    hitMessageStart = 0;
}

void GunClubScene::handleEvent(
    const SDL_Event& event
) {
    if (levelComplete) {
        return;
    }

    if (
        event.type != SDL_KEYDOWN &&
        event.type != SDL_KEYUP
    ) {
        return;
    }

    bool pressed =
        event.type == SDL_KEYDOWN;

    switch (event.key.keysym.sym) {
        case SDLK_LEFT:
        case SDLK_a:
            movingLeft = pressed;
            break;

        case SDLK_RIGHT:
        case SDLK_d:
            movingRight = pressed;
            break;

        default:
            break;
    }
}

void GunClubScene::update() {
    Uint32 currentTime =
        SDL_GetTicks();

    float deltaTime =
        static_cast<float>(
            currentTime -
            lastUpdateTime
        ) / 1000.0f;

    lastUpdateTime =
        currentTime;

    if (deltaTime > 0.05f) {
        deltaTime = 0.05f;
    }

    if (levelComplete) {
        return;
    }

    Uint32 elapsed =
        currentTime -
        sceneStartTime;

    if (
        elapsed >=
        levelDuration
    ) {
        levelComplete = true;

        movingLeft = false;
        movingRight = false;

        return;
    }

    roadScrollOffset +=
        roadSpeed *
        deltaTime;

    damageScrollOffset +=
        roadSpeed *
        0.75f *
        deltaTime;

    while (
        roadScrollOffset >=
        100.0f
    ) {
        roadScrollOffset -=
            100.0f;
    }

    while (
        damageScrollOffset >=
        180.0f
    ) {
        damageScrollOffset -=
            180.0f;
    }

    if (
        movingLeft &&
        !movingRight
    ) {
        playerX -=
            playerSpeed *
            deltaTime;
    }

    if (
        movingRight &&
        !movingLeft
    ) {
        playerX +=
            playerSpeed *
            deltaTime;
    }

    const float minX =
        static_cast<float>(
            roadX + 38
        );

    const float maxX =
        static_cast<float>(
            roadX +
            roadWidth -
            38 -
            playerWidth
        );

    playerX =
        std::clamp(
            playerX,
            minX,
            maxX
        );

    float progress =
        static_cast<float>(
            elapsed
        ) /
        static_cast<float>(
            levelDuration
        );

    Uint32 spawnInterval =
        static_cast<Uint32>(
            1150.0f -
            progress *
            600.0f
        );

    spawnInterval =
        std::max<Uint32>(
            spawnInterval,
            550
        );

    if (
        currentTime -
        lastSpawnTime >=
        spawnInterval
    ) {
        spawnHazardWave();

        lastSpawnTime =
            currentTime;
    }

    updateHazards(
        deltaTime
    );

    updatePotholes(
        deltaTime
    );

    checkCollisions();
}

void GunClubScene::spawnHazardWave() {
    std::array<int, 3> lanes = {
        0,
        1,
        2
    };

    std::shuffle(
        lanes.begin(),
        lanes.end(),
        randomGenerator
    );

    Uint32 elapsed =
        SDL_GetTicks() -
        sceneStartTime;

    float progress =
        static_cast<float>(
            elapsed
        ) /
        static_cast<float>(
            levelDuration
        );

    std::uniform_real_distribution<float>
        chanceDistribution(
            0.0f,
            1.0f
        );

    int hazardCount = 1;

    float doubleChance =
        0.10f +
        progress * 0.65f;

    if (
        chanceDistribution(
            randomGenerator
        ) <
        doubleChance
    ) {
        hazardCount = 2;
    }

    std::uniform_int_distribution<int>
        typeDistribution(
            0,
            2
        );

    for (
        int i = 0;
        i < hazardCount;
        i++
    ) {
        HazardType type =
            static_cast<HazardType>(
                typeDistribution(
                    randomGenerator
                )
            );

        spawnHazard(
            lanes[i],
            type
        );
    }

    // Just like E470, never fill all
    // three lanes in the same wave.
}

void GunClubScene::spawnHazard(
    int lane,
    HazardType type
) {
    ConstructionHazard hazard;

    hazard.type =
        type;

    hazard.lane =
        lane;

    switch (type) {
        case HazardType::Bulldozer:
            hazard.width = 105;
            hazard.height = 105;
            break;

        case HazardType::Cones:
            hazard.width = 90;
            hazard.height = 65;
            break;

        case HazardType::Pipes:
            hazard.width = 110;
            hazard.height = 65;
            break;
    }

    hazard.x =
        getLaneX(
            lane,
            hazard.width
        );

    hazard.y =
        -hazard.height -
        10;

    std::uniform_real_distribution<float>
        speedVariation(
            -20.0f,
            35.0f
        );

    hazard.speed =
        roadSpeed +
        30.0f +
        speedVariation(
            randomGenerator
        );

    hazard.active = true;

    hazards.push_back(
        hazard
    );
}

void GunClubScene::updateHazards(
    float deltaTime
) {
    for (
        ConstructionHazard& hazard :
        hazards
    ) {
        if (!hazard.active) {
            continue;
        }

        hazard.y +=
            hazard.speed *
            deltaTime;

        if (
            hazard.y >
            screenHeight + 130
        ) {
            hazard.active = false;
        }
    }

    hazards.erase(
        std::remove_if(
            hazards.begin(),
            hazards.end(),
            [](
                const ConstructionHazard& hazard
            ) {
                return !hazard.active;
            }
        ),
        hazards.end()
    );
}

void GunClubScene::createPotholes() {
    std::uniform_real_distribution<float>
        xDistribution(
            static_cast<float>(
                roadX + 65
            ),
            static_cast<float>(
                roadX +
                roadWidth -
                110
            )
        );

    std::uniform_int_distribution<int>
        sizeDistribution(
            28,
            55
        );

    const int potholeCount =
        10;

    for (
        int i = 0;
        i < potholeCount;
        i++
    ) {
        Pothole pothole;

        pothole.x =
            xDistribution(
                randomGenerator
            );

        pothole.y =
            static_cast<float>(
                i * 120 - 400
            );

        pothole.width =
            sizeDistribution(
                randomGenerator
            );

        pothole.height =
            pothole.width / 2;

        potholes.push_back(
            pothole
        );
    }
}

void GunClubScene::updatePotholes(
    float deltaTime
) {
    std::uniform_real_distribution<float>
        xDistribution(
            static_cast<float>(
                roadX + 65
            ),
            static_cast<float>(
                roadX +
                roadWidth -
                110
            )
        );

    std::uniform_int_distribution<int>
        sizeDistribution(
            28,
            55
        );

    for (
        Pothole& pothole :
        potholes
    ) {
        pothole.y +=
            roadSpeed *
            deltaTime;

        if (
            pothole.y >
            screenHeight + 60
        ) {
            pothole.y =
                static_cast<float>(
                    -100 -
                    (randomGenerator() %
                     300)
                );

            pothole.x =
                xDistribution(
                    randomGenerator
                );

            pothole.width =
                sizeDistribution(
                    randomGenerator
                );

            pothole.height =
                pothole.width / 2;
        }
    }
}

void GunClubScene::checkCollisions() {
    for (
        ConstructionHazard& hazard :
        hazards
    ) {
        if (!hazard.active) {
            continue;
        }

        if (
            intersects(
                playerX + 8,
                playerY + 8,
                playerWidth - 16,
                playerHeight - 16,
                hazard.x + 5,
                hazard.y + 5,
                hazard.width - 10,
                hazard.height - 10
            )
        ) {
            hits++;

            hitMessageStart =
                SDL_GetTicks();

            hazard.active =
                false;
        }
    }

    // Potholes are currently visual scenery.
    // They make the road look terrible without
    // turning every dark patch into a hidden hitbox.
}

bool GunClubScene::intersects(
    float ax,
    float ay,
    int aw,
    int ah,
    float bx,
    float by,
    int bw,
    int bh
) const {
    return (
        ax <
            bx + bw &&
        ax + aw >
            bx &&
        ay <
            by + bh &&
        ay + ah >
            by
    );
}

float GunClubScene::getLaneX(
    int lane,
    int objectWidth
) const {
    float usableLeft =
        static_cast<float>(
            roadX + 35
        );

    float usableWidth =
        static_cast<float>(
            roadWidth - 70
        );

    float laneWidth =
        usableWidth / 3.0f;

    return
        usableLeft +
        lane * laneWidth +
        (laneWidth -
         objectWidth) / 2.0f;
}

// ----------------------------------------------------
// RENDERING
// ----------------------------------------------------

void GunClubScene::render() {
    renderEnvironment();
    renderHazards();
    renderPlayerCar();
    renderHUD();

    if (levelComplete) {
        renderCompleteScreen();
    }
}

void GunClubScene::renderEnvironment() {
    // Dry roadside dirt.
    SDL_SetRenderDrawColor(
        renderer,
        125,
        107,
        73,
        255
    );

    SDL_Rect background = {
        0,
        0,
        screenWidth,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &background
    );

    renderConstructionScenery();
    renderRoad();
}

void GunClubScene::renderConstructionScenery() {
    int offset =
        static_cast<int>(
            damageScrollOffset
        );

    for (
        int y = -200 + offset;
        y < screenHeight + 200;
        y += 190
    ) {
        // Dirt pile left.
        SDL_SetRenderDrawColor(
            renderer,
            105,
            80,
            48,
            255
        );

        SDL_Rect dirtLeft = {
            roadX - 125,
            y + 35,
            90,
            45
        };

        SDL_RenderFillRect(
            renderer,
            &dirtLeft
        );

        // Dirt pile right.
        SDL_Rect dirtRight = {
            roadX +
            roadWidth +
            35,
            y + 120,
            100,
            42
        };

        SDL_RenderFillRect(
            renderer,
            &dirtRight
        );

        // Temporary orange barrels.
        SDL_SetRenderDrawColor(
            renderer,
            225,
            105,
            20,
            255
        );

        SDL_Rect leftBarrel = {
            roadX - 50,
            y,
            22,
            34
        };

        SDL_Rect rightBarrel = {
            roadX +
            roadWidth +
            28,
            y + 75,
            22,
            34
        };

        SDL_RenderFillRect(
            renderer,
            &leftBarrel
        );

        SDL_RenderFillRect(
            renderer,
            &rightBarrel
        );

        SDL_SetRenderDrawColor(
            renderer,
            245,
            240,
            220,
            255
        );

        SDL_Rect leftStripe = {
            roadX - 50,
            y + 13,
            22,
            5
        };

        SDL_Rect rightStripe = {
            roadX +
            roadWidth +
            28,
            y + 88,
            22,
            5
        };

        SDL_RenderFillRect(
            renderer,
            &leftStripe
        );

        SDL_RenderFillRect(
            renderer,
            &rightStripe
        );
    }
}

void GunClubScene::renderRoad() {
    // Worn-out lighter asphalt.
    SDL_SetRenderDrawColor(
        renderer,
        70,
        68,
        64,
        255
    );

    SDL_Rect road = {
        roadX,
        0,
        roadWidth,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &road
    );

    // Crumbling shoulders.
    SDL_SetRenderDrawColor(
        renderer,
        115,
        105,
        87,
        255
    );

    SDL_Rect leftShoulder = {
        roadX,
        0,
        31,
        screenHeight
    };

    SDL_Rect rightShoulder = {
        roadX +
        roadWidth -
        31,
        0,
        31,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &leftShoulder
    );

    renderRoadDamage();
    renderLaneMarkers();
    renderPotholes();
}

void GunClubScene::renderRoadDamage() {
    int offset =
        static_cast<int>(
            damageScrollOffset
        );

    // Bad repair patches.
    SDL_SetRenderDrawColor(
        renderer,
        51,
        50,
        48,
        255
    );

    for (
        int y = -200 + offset;
        y < screenHeight + 200;
        y += 190
    ) {
        SDL_Rect patchOne = {
            roadX + 80,
            y,
            120,
            28
        };

        SDL_Rect patchTwo = {
            roadX + 370,
            y + 70,
            150,
            34
        };

        SDL_Rect patchThree = {
            roadX + 245,
            y + 130,
            75,
            22
        };

        SDL_RenderFillRect(
            renderer,
            &patchOne
        );

        SDL_RenderFillRect(
            renderer,
            &patchTwo
        );

        SDL_RenderFillRect(
            renderer,
            &patchThree
        );
    }

    // Cracks.
    SDL_SetRenderDrawColor(
        renderer,
        38,
        37,
        36,
        255
    );

    for (
        int y = -100 + offset;
        y < screenHeight;
        y += 150
    ) {
        int x =
            roadX +
            110 +
            (
                (y / 150) %
                4
            ) * 90;

        SDL_RenderDrawLine(
            renderer,
            x,
            y,
            x + 12,
            y + 18
        );

        SDL_RenderDrawLine(
            renderer,
            x + 12,
            y + 18,
            x + 4,
            y + 34
        );

        SDL_RenderDrawLine(
            renderer,
            x + 12,
            y + 18,
            x + 26,
            y + 27
        );
    }

    // Faded/broken edge lines.
    SDL_SetRenderDrawColor(
        renderer,
        195,
        192,
        175,
        255
    );

    for (
        int y = -80 + offset;
        y < screenHeight;
        y += 115
    ) {
        SDL_Rect leftPiece = {
            roadX + 34,
            y,
            5,
            65
        };

        SDL_Rect rightPiece = {
            roadX +
            roadWidth -
            39,
            y + 35,
            5,
            55
        };

        SDL_RenderFillRect(
            renderer,
            &leftPiece
        );

        SDL_RenderFillRect(
            renderer,
            &rightPiece
        );
    }
}

void GunClubScene::renderPotholes() {
    for (
        const Pothole& pothole :
        potholes
    ) {
        int x =
            static_cast<int>(
                pothole.x
            );

        int y =
            static_cast<int>(
                pothole.y
            );

        SDL_SetRenderDrawColor(
            renderer,
            30,
            29,
            28,
            255
        );

        SDL_Rect hole = {
            x,
            y,
            pothole.width,
            pothole.height
        };

        SDL_RenderFillRect(
            renderer,
            &hole
        );

        SDL_SetRenderDrawColor(
            renderer,
            48,
            46,
            43,
            255
        );

        SDL_Rect rim = {
            x + 5,
            y + 3,
            std::max(
                5,
                pothole.width - 10
            ),
            std::max(
                4,
                pothole.height / 3
            )
        };

        SDL_RenderFillRect(
            renderer,
            &rim
        );
    }
}

void GunClubScene::renderLaneMarkers() {
    float usableLeft =
        static_cast<float>(
            roadX + 35
        );

    float usableWidth =
        static_cast<float>(
            roadWidth - 70
        );

    int lane1X =
        static_cast<int>(
            usableLeft +
            usableWidth / 3.0f
        );

    int lane2X =
        static_cast<int>(
            usableLeft +
            usableWidth * 2.0f /
            3.0f
        );

    // Faded lane paint.
    SDL_SetRenderDrawColor(
        renderer,
        185,
        181,
        160,
        255
    );

    int offset =
        static_cast<int>(
            roadScrollOffset
        );

    for (
        int y = -100 + offset;
        y < screenHeight;
        y += 100
    ) {
        SDL_Rect laneOne = {
            lane1X - 3,
            y,
            6,
            45
        };

        SDL_Rect laneTwo = {
            lane2X - 3,
            y + 12,
            6,
            40
        };

        SDL_RenderFillRect(
            renderer,
            &laneOne
        );

        SDL_RenderFillRect(
            renderer,
            &laneTwo
        );
    }
}

// ----------------------------------------------------
// CONSTRUCTION HAZARDS
// ----------------------------------------------------

void GunClubScene::renderHazards() {
    for (
        const ConstructionHazard& hazard :
        hazards
    ) {
        if (!hazard.active) {
            continue;
        }

        switch (hazard.type) {
            case HazardType::Bulldozer:
                drawBulldozer(
                    hazard
                );
                break;

            case HazardType::Cones:
                drawCones(
                    hazard
                );
                break;

            case HazardType::Pipes:
                drawPipes(
                    hazard
                );
                break;
        }
    }
}

void GunClubScene::drawBulldozer(
    const ConstructionHazard& hazard
) {
    int x =
        static_cast<int>(
            hazard.x
        );

    int y =
        static_cast<int>(
            hazard.y
        );

    // Tracks.
    SDL_SetRenderDrawColor(
        renderer,
        45,
        43,
        38,
        255
    );

    SDL_Rect leftTrack = {
        x,
        y + 25,
        20,
        70
    };

    SDL_Rect rightTrack = {
        x +
        hazard.width -
        20,
        y + 25,
        20,
        70
    };

    SDL_RenderFillRect(
        renderer,
        &leftTrack
    );

    SDL_RenderFillRect(
        renderer,
        &rightTrack
    );

    // Yellow body.
    SDL_SetRenderDrawColor(
        renderer,
        225,
        165,
        30,
        255
    );

    SDL_Rect body = {
        x + 18,
        y + 25,
        hazard.width - 36,
        60
    };

    SDL_RenderFillRect(
        renderer,
        &body
    );

    // Cab.
    SDL_SetRenderDrawColor(
        renderer,
        195,
        140,
        25,
        255
    );

    SDL_Rect cab = {
        x + 30,
        y,
        hazard.width - 60,
        42
    };

    SDL_RenderFillRect(
        renderer,
        &cab
    );

    SDL_SetRenderDrawColor(
        renderer,
        35,
        50,
        55,
        255
    );

    SDL_Rect window = {
        x + 38,
        y + 7,
        hazard.width - 76,
        22
    };

    SDL_RenderFillRect(
        renderer,
        &window
    );

    // Bulldozer blade.
    SDL_SetRenderDrawColor(
        renderer,
        145,
        145,
        135,
        255
    );

    SDL_Rect blade = {
        x - 4,
        y + 86,
        hazard.width + 8,
        16
    };

    SDL_RenderFillRect(
        renderer,
        &blade
    );
}

void GunClubScene::drawCones(
    const ConstructionHazard& hazard
) {
    int x =
        static_cast<int>(
            hazard.x
        );

    int y =
        static_cast<int>(
            hazard.y
        );

    const int coneWidth = 22;
    const int coneHeight = 42;

    for (
        int i = 0;
        i < 3;
        i++
    ) {
        int coneX =
            x +
            i * 32;

        SDL_SetRenderDrawColor(
            renderer,
            235,
            105,
            15,
            255
        );

        SDL_Rect cone = {
            coneX + 5,
            y,
            12,
            coneHeight
        };

        SDL_RenderFillRect(
            renderer,
            &cone
        );

        SDL_Rect base = {
            coneX,
            y +
            coneHeight -
            6,
            coneWidth,
            8
        };

        SDL_RenderFillRect(
            renderer,
            &base
        );

        SDL_SetRenderDrawColor(
            renderer,
            245,
            240,
            225,
            255
        );

        SDL_Rect stripe = {
            coneX + 5,
            y + 19,
            12,
            6
        };

        SDL_RenderFillRect(
            renderer,
            &stripe
        );
    }
}

void GunClubScene::drawPipes(
    const ConstructionHazard& hazard
) {
    int x =
        static_cast<int>(
            hazard.x
        );

    int y =
        static_cast<int>(
            hazard.y
        );

    // Pipe bundle shadow.
    SDL_SetRenderDrawColor(
        renderer,
        35,
        35,
        35,
        255
    );

    SDL_Rect shadow = {
        x + 6,
        y + 8,
        hazard.width,
        hazard.height
    };

    SDL_RenderFillRect(
        renderer,
        &shadow
    );

    SDL_SetRenderDrawColor(
        renderer,
        125,
        130,
        132,
        255
    );

    for (
        int i = 0;
        i < 3;
        i++
    ) {
        SDL_Rect pipe = {
            x,
            y + i * 20,
            hazard.width,
            15
        };

        SDL_RenderFillRect(
            renderer,
            &pipe
        );

        SDL_SetRenderDrawColor(
            renderer,
            65,
            68,
            70,
            255
        );

        SDL_Rect pipeEnd = {
            x +
            hazard.width -
            14,
            y +
            i * 20 +
            2,
            10,
            11
        };

        SDL_RenderFillRect(
            renderer,
            &pipeEnd
        );

        SDL_SetRenderDrawColor(
            renderer,
            125,
            130,
            132,
            255
        );
    }
}

// ----------------------------------------------------
// PLAYER
// ----------------------------------------------------

void GunClubScene::renderPlayerCar() {
    int x =
        static_cast<int>(
            playerX
        );

    int y =
        static_cast<int>(
            playerY
        );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        90
    );

    SDL_Rect shadow = {
        x + 8,
        y + 8,
        playerWidth,
        playerHeight
    };

    SDL_RenderFillRect(
        renderer,
        &shadow
    );

    SDL_SetRenderDrawColor(
        renderer,
        115,
        20,
        35,
        255
    );

    SDL_Rect body = {
        x,
        y,
        playerWidth,
        playerHeight
    };

    SDL_RenderFillRect(
        renderer,
        &body
    );

    SDL_SetRenderDrawColor(
        renderer,
        145,
        28,
        42,
        255
    );

    SDL_Rect hood = {
        x + 7,
        y + 5,
        playerWidth - 14,
        25
    };

    SDL_RenderFillRect(
        renderer,
        &hood
    );

    SDL_SetRenderDrawColor(
        renderer,
        30,
        45,
        57,
        255
    );

    SDL_Rect windshield = {
        x + 12,
        y + 34,
        playerWidth - 24,
        34
    };

    SDL_Rect rearWindow = {
        x + 14,
        y + 83,
        playerWidth - 28,
        22
    };

    SDL_RenderFillRect(
        renderer,
        &windshield
    );

    SDL_RenderFillRect(
        renderer,
        &rearWindow
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );
}

// ----------------------------------------------------
// HUD
// ----------------------------------------------------

void GunClubScene::renderHUD() {
    Uint32 elapsed =
        SDL_GetTicks() -
        sceneStartTime;

    Uint32 remaining = 0;

    if (
        elapsed <
        levelDuration
    ) {
        remaining =
            levelDuration -
            elapsed;
    }

    int seconds =
        static_cast<int>(
            std::ceil(
                remaining /
                1000.0f
            )
        );

    textRenderer->renderMessageBox(
        renderer,
        "Gun Club Road - Construction Zone!",
        screenWidth,
        25
    );

    textRenderer->renderFloatingText(
        renderer,
        "TIME: " +
        std::to_string(seconds),
        105,
        95,
        180
    );

    textRenderer->renderFloatingText(
        renderer,
        "HITS: " +
        std::to_string(hits),
        screenWidth - 105,
        95,
        180
    );

    Uint32 currentTime =
        SDL_GetTicks();

    if (
        hitMessageStart != 0 &&
        currentTime -
        hitMessageStart <
        hitMessageDuration
    ) {
        textRenderer->renderFloatingText(
            renderer,
            "HIT!",
            screenWidth / 2,
            145,
            250
        );
    }
}

void GunClubScene::renderCompleteScreen() {
    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        150
    );

    SDL_Rect overlay = {
        0,
        0,
        screenWidth,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &overlay
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );

    textRenderer->renderMessageBox(
        renderer,
        "GUN CLUB ROAD COMPLETE!",
        screenWidth,
        screenHeight / 2 - 70
    );

    textRenderer->renderFloatingText(
        renderer,
        "You survived the construction zone! Hits: " +
        std::to_string(hits),
        screenWidth / 2,
        screenHeight / 2 + 25,
        750
    );
}