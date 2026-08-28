#include "scenes/E470Scene.hpp"

#include <algorithm>
#include <array>
#include <cmath>

E470Scene::E470Scene(
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
      playerSpeed(400.0f),
      movingLeft(false),
      movingRight(false),
      roadScrollOffset(0.0f),
      sceneryScrollOffset(0.0f),
      roadSpeed(350.0f),
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

bool E470Scene::load() {
    return true;
}

void E470Scene::onEnter() {
    clearRequestedScene();

    roadX =
        (screenWidth - roadWidth) / 2;

    playerX =
        static_cast<float>(
            (screenWidth - playerWidth) / 2
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
    sceneryScrollOffset = 0.0f;

    traffic.clear();

    hits = 0;

    levelComplete = false;

    sceneStartTime = SDL_GetTicks();
    lastUpdateTime = sceneStartTime;
    lastSpawnTime = sceneStartTime;

    hitMessageStart = 0;
}

void E470Scene::handleEvent(
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

void E470Scene::update() {
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

    if (elapsed >= levelDuration) {
        levelComplete = true;

        movingLeft = false;
        movingRight = false;

        return;
    }

    roadScrollOffset +=
        roadSpeed *
        deltaTime;

    sceneryScrollOffset +=
        roadSpeed *
        0.7f *
        deltaTime;

    while (
        roadScrollOffset >=
        100.0f
    ) {
        roadScrollOffset -=
            100.0f;
    }

    while (
        sceneryScrollOffset >=
        160.0f
    ) {
        sceneryScrollOffset -=
            160.0f;
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

    const float minPlayerX =
        static_cast<float>(
            roadX + 38
        );

    const float maxPlayerX =
        static_cast<float>(
            roadX +
            roadWidth -
            38 -
            playerWidth
        );

    playerX =
        std::clamp(
            playerX,
            minPlayerX,
            maxPlayerX
        );

    // Spawn faster as the 30-second level progresses.
    float progress =
        static_cast<float>(elapsed) /
        static_cast<float>(levelDuration);

    Uint32 spawnInterval =
        static_cast<Uint32>(
            1050.0f -
            progress * 550.0f
        );

    spawnInterval =
        std::max<Uint32>(
            spawnInterval,
            500
        );

    if (
        currentTime -
        lastSpawnTime >=
        spawnInterval
    ) {
        spawnTrafficWave();

        lastSpawnTime =
            currentTime;
    }

    updateTraffic(
        deltaTime
    );

    checkCollisions();
}

void E470Scene::spawnTrafficWave() {
    Uint32 elapsed =
        SDL_GetTicks() -
        sceneStartTime;

    float progress =
        static_cast<float>(elapsed) /
        static_cast<float>(levelDuration);

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

    // Early game: almost always one car.
    // Later: more two-car waves.
    std::uniform_real_distribution<float>
        chanceDistribution(
            0.0f,
            1.0f
        );

    float twoCarChance =
        0.15f +
        progress * 0.65f;

    int carsThisWave = 1;

    if (
        chanceDistribution(
            randomGenerator
        ) <
        twoCarChance
    ) {
        carsThisWave = 2;
    }

    // Never spawn all three lanes at once.
    // There is always at least one open route.
    for (
        int i = 0;
        i < carsThisWave;
        i++
    ) {
        spawnCar(
            lanes[i],
            0.0f
        );
    }
}

void E470Scene::spawnCar(
    int lane,
    float yOffset
) {
    TrafficCar car;

    car.width = 66;
    car.height = 105;

    car.lane = lane;

    car.x =
        getLaneX(
            lane,
            car.width
        );

    car.y =
        -car.height -
        yOffset;

    Uint32 elapsed =
        SDL_GetTicks() -
        sceneStartTime;

    float progress =
        static_cast<float>(elapsed) /
        static_cast<float>(levelDuration);

    std::uniform_real_distribution<float>
        speedVariation(
            -25.0f,
            55.0f
        );

    car.speed =
        roadSpeed +
        45.0f +
        progress * 80.0f +
        speedVariation(
            randomGenerator
        );

    // Randomized car colors.
    static const std::array<
        std::array<Uint8, 3>,
        7
    > colors = {{
        {45, 90, 170},
        {185, 45, 45},
        {190, 190, 195},
        {35, 35, 40},
        {210, 150, 40},
        {45, 125, 85},
        {135, 70, 155}
    }};

    std::uniform_int_distribution<int>
        colorDistribution(
            0,
            static_cast<int>(
                colors.size() - 1
            )
        );

    int colorIndex =
        colorDistribution(
            randomGenerator
        );

    car.red =
        colors[colorIndex][0];

    car.green =
        colors[colorIndex][1];

    car.blue =
        colors[colorIndex][2];

    car.active = true;

    traffic.push_back(
        car
    );
}

void E470Scene::updateTraffic(
    float deltaTime
) {
    for (
        TrafficCar& car :
        traffic
    ) {
        if (!car.active) {
            continue;
        }

        car.y +=
            car.speed *
            deltaTime;

        if (
            car.y >
            screenHeight + 120
        ) {
            car.active = false;
        }
    }

    traffic.erase(
        std::remove_if(
            traffic.begin(),
            traffic.end(),
            [](const TrafficCar& car) {
                return !car.active;
            }
        ),
        traffic.end()
    );
}

void E470Scene::checkCollisions() {
    for (
        TrafficCar& car :
        traffic
    ) {
        if (!car.active) {
            continue;
        }

        // Slightly smaller collision box
        // keeps the dodging feeling fair.
        if (
            intersects(
                playerX + 8,
                playerY + 8,
                playerWidth - 16,
                playerHeight - 16,
                car.x + 6,
                car.y + 6,
                car.width - 12,
                car.height - 12
            )
        ) {
            hits++;

            hitMessageStart =
                SDL_GetTicks();

            car.active = false;
        }
    }
}

bool E470Scene::intersects(
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

float E470Scene::getLaneX(
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

void E470Scene::render() {
    renderEnvironment();
    renderTraffic();
    renderPlayerCar();
    renderHUD();

    if (levelComplete) {
        renderCompleteScreen();
    }
}

void E470Scene::renderEnvironment() {
    // Grass.
    SDL_SetRenderDrawColor(
        renderer,
        47,
        103,
        47,
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

    renderRoadside();
    renderRoad();
}

void E470Scene::renderRoadside() {
    int offset =
        static_cast<int>(
            sceneryScrollOffset
        );

    for (
        int y = -180 + offset;
        y < screenHeight + 180;
        y += 180
    ) {
        // Left trees.
        SDL_SetRenderDrawColor(
            renderer,
            32,
            82,
            34,
            255
        );

        SDL_Rect leftTree = {
            roadX - 115,
            y,
            52,
            70
        };

        SDL_RenderFillRect(
            renderer,
            &leftTree
        );

        SDL_SetRenderDrawColor(
            renderer,
            45,
            120,
            48,
            255
        );

        SDL_Rect leftLeaves = {
            roadX - 130,
            y - 20,
            82,
            58
        };

        SDL_RenderFillRect(
            renderer,
            &leftLeaves
        );

        // Right tree.
        SDL_SetRenderDrawColor(
            renderer,
            32,
            82,
            34,
            255
        );

        SDL_Rect rightTree = {
            roadX +
            roadWidth +
            65,
            y + 80,
            52,
            70
        };

        SDL_RenderFillRect(
            renderer,
            &rightTree
        );

        SDL_SetRenderDrawColor(
            renderer,
            45,
            120,
            48,
            255
        );

        SDL_Rect rightLeaves = {
            roadX +
            roadWidth +
            50,
            y + 60,
            82,
            58
        };

        SDL_RenderFillRect(
            renderer,
            &rightLeaves
        );
    }
}

void E470Scene::renderRoad() {
    SDL_SetRenderDrawColor(
        renderer,
        54,
        57,
        60,
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

    // Shoulders.
    SDL_SetRenderDrawColor(
        renderer,
        105,
        103,
        92,
        255
    );

    SDL_Rect leftShoulder = {
        roadX,
        0,
        25,
        screenHeight
    };

    SDL_Rect rightShoulder = {
        roadX +
        roadWidth -
        25,
        0,
        25,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &leftShoulder
    );

    SDL_RenderFillRect(
        renderer,
        &rightShoulder
    );

    // White road edges.
    SDL_SetRenderDrawColor(
        renderer,
        240,
        240,
        232,
        255
    );

    SDL_Rect leftEdge = {
        roadX + 29,
        0,
        6,
        screenHeight
    };

    SDL_Rect rightEdge = {
        roadX +
        roadWidth -
        35,
        0,
        6,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &leftEdge
    );

    SDL_RenderFillRect(
        renderer,
        &rightEdge
    );

    renderLaneMarkers();
}

void E470Scene::renderLaneMarkers() {
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
            usableWidth * 2.0f / 3.0f
        );

    SDL_SetRenderDrawColor(
        renderer,
        245,
        245,
        225,
        255
    );

    const int dashHeight = 55;
    const int cycle = 100;

    int offset =
        static_cast<int>(
            roadScrollOffset
        );

    for (
        int y =
            -cycle + offset;
        y < screenHeight;
        y += cycle
    ) {
        SDL_Rect laneOne = {
            lane1X - 3,
            y,
            6,
            dashHeight
        };

        SDL_Rect laneTwo = {
            lane2X - 3,
            y,
            6,
            dashHeight
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

void E470Scene::renderTraffic() {
    for (
        const TrafficCar& car :
        traffic
    ) {
        if (car.active) {
            renderTrafficCar(
                car
            );
        }
    }
}

void E470Scene::renderTrafficCar(
    const TrafficCar& car
) {
    int x =
        static_cast<int>(
            car.x
        );

    int y =
        static_cast<int>(
            car.y
        );

    // Shadow.
    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        80
    );

    SDL_Rect shadow = {
        x + 7,
        y + 8,
        car.width,
        car.height
    };

    SDL_RenderFillRect(
        renderer,
        &shadow
    );

    SDL_SetRenderDrawColor(
        renderer,
        car.red,
        car.green,
        car.blue,
        255
    );

    SDL_Rect body = {
        x,
        y,
        car.width,
        car.height
    };

    SDL_RenderFillRect(
        renderer,
        &body
    );

    SDL_SetRenderDrawColor(
        renderer,
        32,
        44,
        54,
        255
    );

    SDL_Rect frontWindow = {
        x + 10,
        y + 18,
        car.width - 20,
        28
    };

    SDL_Rect rearWindow = {
        x + 12,
        y + 65,
        car.width - 24,
        22
    };

    SDL_RenderFillRect(
        renderer,
        &frontWindow
    );

    SDL_RenderFillRect(
        renderer,
        &rearWindow
    );

    SDL_SetRenderDrawColor(
        renderer,
        220,
        35,
        35,
        255
    );

    SDL_Rect leftBrake = {
        x + 7,
        y + car.height - 8,
        13,
        6
    };

    SDL_Rect rightBrake = {
        x + car.width - 20,
        y + car.height - 8,
        13,
        6
    };

    SDL_RenderFillRect(
        renderer,
        &leftBrake
    );

    SDL_RenderFillRect(
        renderer,
        &rightBrake
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );
}

void E470Scene::renderPlayerCar() {
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

void E470Scene::renderHUD() {
    Uint32 elapsed =
        SDL_GetTicks() -
        sceneStartTime;

    Uint32 remaining =
        0;

    if (elapsed < levelDuration) {
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
        "E470 - Dodge the traffic!",
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

void E470Scene::renderCompleteScreen() {
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
        "E470 COMPLETE!",
        screenWidth,
        screenHeight / 2 - 70
    );

    textRenderer->renderFloatingText(
        renderer,
        "You made it through traffic! Hits: " +
        std::to_string(hits),
        screenWidth / 2,
        screenHeight / 2 + 25,
        650
    );
}