#include "scenes/DrivingScene.hpp"

#include <algorithm>
#include <cmath>

DrivingScene::DrivingScene(
    SDL_Renderer* renderer,
    TextRenderer* textRenderer,
    int screenWidth,
    int screenHeight
)
    : renderer(renderer),
      textRenderer(textRenderer),
      screenWidth(screenWidth),
      screenHeight(screenHeight),
      carX(0.0f),
      carY(0.0f),
      carWidth(70),
      carHeight(120),
      carMoveSpeed(330.0f),
      movingLeft(false),
      movingRight(false),
      controlsEnabled(false),
      forkActive(false),
      routeCommitted(false),
      selectedRoute(SceneType::None),
      sceneStartTime(0),
      lastUpdateTime(0),
      routeCommitTime(0),
      controlUnlockDelay(2000),
      forkStartDelay(5500),
      forkAnimationDuration(2800),
      routeDisplayDuration(900),
      forkProgress(0.0f),
      laneScrollOffset(0.0f),
      roadTextureOffset(0.0f),
      guardRailOffset(0.0f),
      roadScrollSpeed(320.0f),
      roadWidth(620),
      randomGenerator(std::random_device{}()) {
}

bool DrivingScene::load() {
    // Everything in this scene is currently
    // drawn directly with SDL.
    return true;
}

void DrivingScene::onEnter() {
    clearRequestedScene();

    carX =
        static_cast<float>(
            (screenWidth - carWidth) / 2
        );

    carY =
        static_cast<float>(
            screenHeight -
            carHeight -
            55
        );

    movingLeft = false;
    movingRight = false;

    controlsEnabled = false;
    forkActive = false;
    routeCommitted = false;

    selectedRoute = SceneType::None;

    forkProgress = 0.0f;

    laneScrollOffset = 0.0f;
    roadTextureOffset = 0.0f;
    guardRailOffset = 0.0f;

    sceneStartTime = SDL_GetTicks();
    lastUpdateTime = sceneStartTime;

    routeCommitTime = 0;

    initializeRoadsideObjects();
}

void DrivingScene::handleEvent(
    const SDL_Event& event
) {
    if (
        !controlsEnabled ||
        routeCommitted
    ) {
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

void DrivingScene::update() {
    Uint32 currentTime =
        SDL_GetTicks();

    float deltaTime =
        static_cast<float>(
            currentTime -
            lastUpdateTime
        ) / 1000.0f;

    lastUpdateTime =
        currentTime;

    Uint32 elapsedTime =
        currentTime -
        sceneStartTime;

    laneScrollOffset +=
        roadScrollSpeed *
        deltaTime;

    roadTextureOffset +=
        roadScrollSpeed *
        0.55f *
        deltaTime;

    guardRailOffset +=
        roadScrollSpeed *
        deltaTime;

    while (
        laneScrollOffset >=
        100.0f
    ) {
        laneScrollOffset -=
            100.0f;
    }

    while (
        roadTextureOffset >=
        140.0f
    ) {
        roadTextureOffset -=
            140.0f;
    }

    while (
        guardRailOffset >=
        80.0f
    ) {
        guardRailOffset -=
            80.0f;
    }

    updateRoadsideObjects(
        deltaTime
    );

    if (
        !controlsEnabled &&
        elapsedTime >=
        controlUnlockDelay
    ) {
        controlsEnabled = true;
    }

    if (
        !forkActive &&
        elapsedTime >=
        forkStartDelay
    ) {
        forkActive = true;
    }

    if (forkActive) {
        float elapsedForkTime =
            static_cast<float>(
                elapsedTime -
                forkStartDelay
            );

        forkProgress =
            elapsedForkTime /
            static_cast<float>(
                forkAnimationDuration
            );

        forkProgress =
            std::clamp(
                forkProgress,
                0.0f,
                1.0f
            );
    }

    if (
        controlsEnabled &&
        !routeCommitted
    ) {
        if (
            movingLeft &&
            !movingRight
        ) {
            carX -=
                carMoveSpeed *
                deltaTime;
        }

        if (
            movingRight &&
            !movingLeft
        ) {
            carX +=
                carMoveSpeed *
                deltaTime;
        }

        float minCarX =
            110.0f;

        float maxCarX =
            static_cast<float>(
                screenWidth -
                carWidth -
                110
            );

        carX =
            std::clamp(
                carX,
                minCarX,
                maxCarX
            );
    }

    // Once the fork reaches the car,
    // horizontal position determines
    // the chosen route.
    if (
        forkActive &&
        forkProgress >= 1.0f &&
        !routeCommitted
    ) {
        float carCenter =
            carX +
            carWidth / 2.0f;

        float screenCenter =
            screenWidth / 2.0f;

        const float decisionDeadZone =
            70.0f;

        if (
            carCenter <
            screenCenter -
            decisionDeadZone
        ) {
            commitRoute(
                SceneType::E470
            );
        }
        else if (
            carCenter >
            screenCenter +
            decisionDeadZone
        ) {
            commitRoute(
                SceneType::GunClub
            );
        }
    }

    if (routeCommitted) {
        if (
            currentTime -
            routeCommitTime >=
            routeDisplayDuration
        ) {
            requestedScene =
                selectedRoute;
        }
    }
}

void DrivingScene::commitRoute(
    SceneType route
) {
    if (routeCommitted) {
        return;
    }

    selectedRoute =
        route;

    routeCommitted =
        true;

    routeCommitTime =
        SDL_GetTicks();

    movingLeft = false;
    movingRight = false;
}

void DrivingScene::render() {
    renderRoad();

    renderRoadsideScenery();

    if (!forkActive) {
        drawGuardRails();
    }

    renderCar();

    renderInstructions();
}

// ----------------------------------------------------
// ROAD
// ----------------------------------------------------

void DrivingScene::renderRoad() {
    // Base roadside grass.
    SDL_SetRenderDrawColor(
        renderer,
        55,
        108,
        47,
        255
    );

    SDL_Rect backgroundRect = {
        0,
        0,
        screenWidth,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &backgroundRect
    );

    // Slight grass variation.
    SDL_SetRenderDrawColor(
        renderer,
        62,
        118,
        53,
        255
    );

    for (
        int y = -120;
        y < screenHeight + 120;
        y += 150
    ) {
        int offset =
            static_cast<int>(
                roadTextureOffset
            );

        SDL_Rect leftPatch = {
            25,
            y + offset,
            120,
            45
        };

        SDL_Rect rightPatch = {
            screenWidth - 165,
            y + offset + 60,
            135,
            40
        };

        SDL_RenderFillRect(
            renderer,
            &leftPatch
        );

        SDL_RenderFillRect(
            renderer,
            &rightPatch
        );
    }

    if (forkActive) {
        renderForkRoad();
    }
    else {
        renderStraightRoad();
    }
}

void DrivingScene::renderStraightRoad() {
    int roadX =
        (screenWidth -
         roadWidth) / 2;

    // Dark asphalt.
    SDL_SetRenderDrawColor(
        renderer,
        55,
        58,
        61,
        255
    );

    SDL_Rect roadRect = {
        roadX,
        0,
        roadWidth,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &roadRect
    );

    renderRoadShoulders();
    renderRoadTexture();
    renderLaneMarkers();
}

void DrivingScene::renderRoadShoulders() {
    int roadX =
        (screenWidth -
         roadWidth) / 2;

    // Gravel shoulder.
    SDL_SetRenderDrawColor(
        renderer,
        112,
        108,
        94,
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

    // White road-edge lines.
    SDL_SetRenderDrawColor(
        renderer,
        238,
        238,
        230,
        255
    );

    SDL_Rect leftEdge = {
        roadX + 28,
        0,
        6,
        screenHeight
    };

    SDL_Rect rightEdge = {
        roadX +
        roadWidth -
        34,
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
}

void DrivingScene::renderRoadTexture() {
    int roadX =
        (screenWidth -
         roadWidth) / 2;

    // Subtle asphalt repair patches.
    SDL_SetRenderDrawColor(
        renderer,
        48,
        50,
        53,
        255
    );

    int offset =
        static_cast<int>(
            roadTextureOffset
        );

    for (
        int y = -160 + offset;
        y < screenHeight + 160;
        y += 170
    ) {
        SDL_Rect patch1 = {
            roadX + 100,
            y,
            75,
            18
        };

        SDL_Rect patch2 = {
            roadX + 390,
            y + 65,
            105,
            15
        };

        SDL_RenderFillRect(
            renderer,
            &patch1
        );

        SDL_RenderFillRect(
            renderer,
            &patch2
        );
    }

    // Small road cracks.
    SDL_SetRenderDrawColor(
        renderer,
        38,
        40,
        42,
        255
    );

    for (
        int y = -100 + offset;
        y < screenHeight;
        y += 220
    ) {
        SDL_RenderDrawLine(
            renderer,
            roadX + 250,
            y,
            roadX + 260,
            y + 15
        );

        SDL_RenderDrawLine(
            renderer,
            roadX + 260,
            y + 15,
            roadX + 253,
            y + 28
        );
    }
}

void DrivingScene::renderLaneMarkers() {
    int roadX =
        (screenWidth -
         roadWidth) / 2;

    int lane1X =
        roadX +
        roadWidth / 3;

    int lane2X =
        roadX +
        (roadWidth * 2) / 3;

    SDL_SetRenderDrawColor(
        renderer,
        245,
        245,
        220,
        255
    );

    const int dashHeight = 55;
    const int gapHeight = 45;
    const int cycle =
        dashHeight +
        gapHeight;

    int offset =
        static_cast<int>(
            laneScrollOffset
        );

    for (
        int y =
            -cycle + offset;
        y < screenHeight;
        y += cycle
    ) {
        SDL_Rect lane1 = {
            lane1X - 3,
            y,
            6,
            dashHeight
        };

        SDL_Rect lane2 = {
            lane2X - 3,
            y,
            6,
            dashHeight
        };

        SDL_RenderFillRect(
            renderer,
            &lane1
        );

        SDL_RenderFillRect(
            renderer,
            &lane2
        );
    }
}

// ----------------------------------------------------
// FORK
// ----------------------------------------------------

void DrivingScene::renderForkRoad() {
    int centerX =
        screenWidth / 2;

    int straightRoadX =
        centerX -
        roadWidth / 2;

    int targetTipY =
        static_cast<int>(
            carY - 70
        );

    int splitTipY =
        static_cast<int>(
            -120 +
            forkProgress *
            (targetTipY + 120)
        );

    splitTipY =
        std::max(
            splitTipY,
            0
        );

    SDL_SetRenderDrawColor(
        renderer,
        55,
        58,
        61,
        255
    );

    // Lower straight road.
    SDL_Rect lowerRoad = {
        straightRoadX,
        splitTipY,
        roadWidth,
        screenHeight -
        splitTipY
    };

    SDL_RenderFillRect(
        renderer,
        &lowerRoad
    );

    const int stripHeight = 4;

    const int branchWidth =
        roadWidth / 2 -
        25;

    const float maxSpread =
        285.0f;

    for (
        int y = 0;
        y < splitTipY;
        y += stripHeight
    ) {
        float distanceFromTip =
            static_cast<float>(
                splitTipY - y
            ) /
            static_cast<float>(
                std::max(
                    splitTipY,
                    1
                )
            );

        int spread =
            static_cast<int>(
                maxSpread *
                distanceFromTip
            );

        int leftCenter =
            centerX - spread;

        int rightCenter =
            centerX + spread;

        SDL_Rect leftBranch = {
            leftCenter -
            branchWidth / 2,
            y,
            branchWidth,
            stripHeight
        };

        SDL_Rect rightBranch = {
            rightCenter -
            branchWidth / 2,
            y,
            branchWidth,
            stripHeight
        };

        SDL_RenderFillRect(
            renderer,
            &leftBranch
        );

        SDL_RenderFillRect(
            renderer,
            &rightBranch
        );
    }

    // Edge lines on the lower road.
    SDL_SetRenderDrawColor(
        renderer,
        238,
        238,
        230,
        255
    );

    SDL_Rect leftEdge = {
        straightRoadX + 28,
        splitTipY,
        6,
        screenHeight -
        splitTipY
    };

    SDL_Rect rightEdge = {
        straightRoadX +
        roadWidth -
        34,
        splitTipY,
        6,
        screenHeight -
        splitTipY
    };

    SDL_RenderFillRect(
        renderer,
        &leftEdge
    );

    SDL_RenderFillRect(
        renderer,
        &rightEdge
    );

    if (splitTipY > 0) {
        renderSigns();
    }
}

// ----------------------------------------------------
// ROADSIDE OBJECTS
// ----------------------------------------------------

void DrivingScene::initializeRoadsideObjects() {
    roadsideObjects.clear();

    const int objectCount = 20;

    for (
        int i = 0;
        i < objectCount;
        i++
    ) {
        RoadsideObject object;

        object.leftSide =
            (i % 2 == 0);

        object.y =
            static_cast<float>(
                (i * screenHeight /
                 objectCount) -
                150
            );

        resetRoadsideObject(
            object,
            false
        );

        object.y =
            static_cast<float>(
                (i * 100) -
                250
            );

        roadsideObjects.push_back(
            object
        );
    }
}

void DrivingScene::updateRoadsideObjects(
    float deltaTime
) {
    for (
        RoadsideObject& object :
        roadsideObjects
    ) {
        object.y +=
            roadScrollSpeed *
            deltaTime;

        if (
            object.y >
            screenHeight + 120
        ) {
            resetRoadsideObject(
                object,
                true
            );
        }
    }
}

void DrivingScene::resetRoadsideObject(
    RoadsideObject& object,
    bool placeAboveScreen
) {
    std::uniform_int_distribution<int>
        typeDistribution(0, 5);

    std::uniform_real_distribution<float>
        scaleDistribution(
            0.75f,
            1.25f
        );

    std::uniform_int_distribution<int>
        sideDistribution(0, 1);

    std::uniform_int_distribution<int>
        distanceDistribution(
            45,
            220
        );

    std::uniform_int_distribution<int>
        verticalDistribution(
            70,
            250
        );

    object.type =
        static_cast<
            RoadsideObjectType
        >(
            typeDistribution(
                randomGenerator
            )
        );

    object.scale =
        scaleDistribution(
            randomGenerator
        );

    object.leftSide =
        sideDistribution(
            randomGenerator
        ) == 0;

    int roadX =
        (screenWidth -
         roadWidth) / 2;

    int distance =
        distanceDistribution(
            randomGenerator
        );

    if (object.leftSide) {
        object.x =
            static_cast<float>(
                roadX -
                distance
            );
    }
    else {
        object.x =
            static_cast<float>(
                roadX +
                roadWidth +
                distance
            );
    }

    if (placeAboveScreen) {
        object.y =
            static_cast<float>(
                -verticalDistribution(
                    randomGenerator
                )
            );
    }
}

void DrivingScene::renderRoadsideScenery() {
    for (
        const RoadsideObject& object :
        roadsideObjects
    ) {
        renderRoadsideObject(
            object
        );
    }
}

void DrivingScene::renderRoadsideObject(
    const RoadsideObject& object
) {
    int x =
        static_cast<int>(
            object.x
        );

    int y =
        static_cast<int>(
            object.y
        );

    switch (object.type) {
        case RoadsideObjectType::Tree:
            drawTree(
                x,
                y,
                object.scale
            );
            break;

        case RoadsideObjectType::Bush:
            drawBush(
                x,
                y,
                object.scale
            );
            break;

        case RoadsideObjectType::Rock:
            drawRock(
                x,
                y,
                object.scale
            );
            break;

        case RoadsideObjectType::SpeedSign:
            drawSpeedSign(
                x,
                y,
                object.scale
            );
            break;

        case RoadsideObjectType::WarningSign:
            drawWarningSign(
                x,
                y,
                object.scale
            );
            break;

        case RoadsideObjectType::UtilityPole:
            drawUtilityPole(
                x,
                y,
                object.scale
            );
            break;
    }
}

// ----------------------------------------------------
// TREE
// ----------------------------------------------------

void DrivingScene::drawTree(
    int x,
    int y,
    float scale
) {
    int trunkWidth =
        static_cast<int>(
            14 * scale
        );

    int trunkHeight =
        static_cast<int>(
            35 * scale
        );

    int canopySize =
        static_cast<int>(
            48 * scale
        );

    // Tree shadow.
    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        20,
        40,
        20,
        90
    );

    SDL_Rect shadow = {
        x -
        canopySize / 2 +
        8,
        y +
        canopySize / 3,
        canopySize,
        canopySize / 2
    };

    SDL_RenderFillRect(
        renderer,
        &shadow
    );

    // Trunk.
    SDL_SetRenderDrawColor(
        renderer,
        92,
        62,
        35,
        255
    );

    SDL_Rect trunk = {
        x -
        trunkWidth / 2,
        y,
        trunkWidth,
        trunkHeight
    };

    SDL_RenderFillRect(
        renderer,
        &trunk
    );

    // Dark canopy.
    SDL_SetRenderDrawColor(
        renderer,
        30,
        92,
        38,
        255
    );

    SDL_Rect canopyBack = {
        x -
        canopySize / 2,
        y -
        canopySize / 2,
        canopySize,
        canopySize
    };

    SDL_RenderFillRect(
        renderer,
        &canopyBack
    );

    // Lighter foliage.
    SDL_SetRenderDrawColor(
        renderer,
        46,
        125,
        52,
        255
    );

    int smallerSize =
        static_cast<int>(
            canopySize *
            0.65f
        );

    SDL_Rect canopyFront = {
        x -
        smallerSize / 2 -
        6,
        y -
        canopySize / 2 +
        5,
        smallerSize,
        smallerSize
    };

    SDL_RenderFillRect(
        renderer,
        &canopyFront
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );
}

// ----------------------------------------------------
// BUSH
// ----------------------------------------------------

void DrivingScene::drawBush(
    int x,
    int y,
    float scale
) {
    int width =
        static_cast<int>(
            45 * scale
        );

    int height =
        static_cast<int>(
            30 * scale
        );

    SDL_SetRenderDrawColor(
        renderer,
        28,
        88,
        34,
        255
    );

    SDL_Rect bushBack = {
        x - width / 2,
        y - height / 2,
        width,
        height
    };

    SDL_RenderFillRect(
        renderer,
        &bushBack
    );

    SDL_SetRenderDrawColor(
        renderer,
        54,
        135,
        54,
        255
    );

    SDL_Rect bushFront = {
        x - width / 3,
        y - height / 2 - 4,
        width / 2,
        height / 2
    };

    SDL_RenderFillRect(
        renderer,
        &bushFront
    );
}

// ----------------------------------------------------
// ROCK
// ----------------------------------------------------

void DrivingScene::drawRock(
    int x,
    int y,
    float scale
) {
    int width =
        static_cast<int>(
            34 * scale
        );

    int height =
        static_cast<int>(
            25 * scale
        );

    SDL_SetRenderDrawColor(
        renderer,
        96,
        96,
        90,
        255
    );

    SDL_Rect rock = {
        x - width / 2,
        y - height / 2,
        width,
        height
    };

    SDL_RenderFillRect(
        renderer,
        &rock
    );

    SDL_SetRenderDrawColor(
        renderer,
        130,
        130,
        120,
        255
    );

    SDL_Rect highlight = {
        x - width / 3,
        y - height / 3,
        width / 3,
        height / 4
    };

    SDL_RenderFillRect(
        renderer,
        &highlight
    );
}

// ----------------------------------------------------
// SPEED LIMIT SIGN
// ----------------------------------------------------

void DrivingScene::drawSpeedSign(
    int x,
    int y,
    float scale
) {
    int poleWidth =
        std::max(
            3,
            static_cast<int>(
                4 * scale
            )
        );

    int poleHeight =
        static_cast<int>(
            55 * scale
        );

    int signWidth =
        static_cast<int>(
            40 * scale
        );

    int signHeight =
        static_cast<int>(
            48 * scale
        );

    SDL_SetRenderDrawColor(
        renderer,
        100,
        100,
        100,
        255
    );

    SDL_Rect pole = {
        x - poleWidth / 2,
        y,
        poleWidth,
        poleHeight
    };

    SDL_RenderFillRect(
        renderer,
        &pole
    );

    SDL_SetRenderDrawColor(
        renderer,
        238,
        238,
        232,
        255
    );

    SDL_Rect sign = {
        x - signWidth / 2,
        y - signHeight,
        signWidth,
        signHeight
    };

    SDL_RenderFillRect(
        renderer,
        &sign
    );

    SDL_SetRenderDrawColor(
        renderer,
        35,
        35,
        35,
        255
    );

    SDL_RenderDrawRect(
        renderer,
        &sign
    );

    // Simple "65" shape.
    int centerX = x;

    SDL_Rect topBar = {
        centerX - 10,
        y - signHeight + 12,
        20,
        4
    };

    SDL_Rect middleBar = {
        centerX - 10,
        y - signHeight + 22,
        20,
        4
    };

    SDL_Rect bottomBar = {
        centerX - 10,
        y - signHeight + 32,
        20,
        4
    };

    SDL_RenderFillRect(
        renderer,
        &topBar
    );

    SDL_RenderFillRect(
        renderer,
        &middleBar
    );

    SDL_RenderFillRect(
        renderer,
        &bottomBar
    );
}

// ----------------------------------------------------
// WARNING SIGN
// ----------------------------------------------------

void DrivingScene::drawWarningSign(
    int x,
    int y,
    float scale
) {
    int poleHeight =
        static_cast<int>(
            55 * scale
        );

    int signSize =
        static_cast<int>(
            38 * scale
        );

    SDL_SetRenderDrawColor(
        renderer,
        90,
        90,
        90,
        255
    );

    SDL_Rect pole = {
        x - 2,
        y,
        4,
        poleHeight
    };

    SDL_RenderFillRect(
        renderer,
        &pole
    );

    // Diamond approximation.
    SDL_SetRenderDrawColor(
        renderer,
        235,
        185,
        35,
        255
    );

    SDL_Rect diamondCenter = {
        x -
        signSize / 3,
        y -
        signSize,
        (signSize * 2) / 3,
        signSize
    };

    SDL_RenderFillRect(
        renderer,
        &diamondCenter
    );

    SDL_Rect diamondMiddle = {
        x -
        signSize / 2,
        y -
        signSize +
        signSize / 4,
        signSize,
        signSize / 2
    };

    SDL_RenderFillRect(
        renderer,
        &diamondMiddle
    );

    SDL_SetRenderDrawColor(
        renderer,
        35,
        35,
        35,
        255
    );

    SDL_Rect warningMark = {
        x - 2,
        y -
        signSize +
        10,
        4,
        15
    };

    SDL_RenderFillRect(
        renderer,
        &warningMark
    );
}

// ----------------------------------------------------
// UTILITY POLE
// ----------------------------------------------------

void DrivingScene::drawUtilityPole(
    int x,
    int y,
    float scale
) {
    int poleHeight =
        static_cast<int>(
            95 * scale
        );

    int poleWidth =
        std::max(
            5,
            static_cast<int>(
                7 * scale
            )
        );

    SDL_SetRenderDrawColor(
        renderer,
        88,
        60,
        38,
        255
    );

    SDL_Rect pole = {
        x - poleWidth / 2,
        y - poleHeight,
        poleWidth,
        poleHeight
    };

    SDL_RenderFillRect(
        renderer,
        &pole
    );

    int crossWidth =
        static_cast<int>(
            48 * scale
        );

    SDL_Rect crossBar = {
        x - crossWidth / 2,
        y - poleHeight + 15,
        crossWidth,
        5
    };

    SDL_RenderFillRect(
        renderer,
        &crossBar
    );

    SDL_SetRenderDrawColor(
        renderer,
        35,
        35,
        35,
        255
    );

    SDL_RenderDrawLine(
        renderer,
        x -
        crossWidth / 2,
        y -
        poleHeight +
        17,
        x -
        crossWidth / 2 -
        30,
        y -
        poleHeight +
        25
    );

    SDL_RenderDrawLine(
        renderer,
        x +
        crossWidth / 2,
        y -
        poleHeight +
        17,
        x +
        crossWidth / 2 +
        30,
        y -
        poleHeight +
        25
    );
}

// ----------------------------------------------------
// GUARDRAILS
// ----------------------------------------------------

void DrivingScene::drawGuardRails() {
    int roadX =
        (screenWidth -
         roadWidth) / 2;

    int leftRailX =
        roadX - 18;

    int rightRailX =
        roadX +
        roadWidth +
        12;

    SDL_SetRenderDrawColor(
        renderer,
        165,
        168,
        170,
        255
    );

    SDL_Rect leftRail = {
        leftRailX,
        0,
        6,
        screenHeight
    };

    SDL_Rect rightRail = {
        rightRailX,
        0,
        6,
        screenHeight
    };

    SDL_RenderFillRect(
        renderer,
        &leftRail
    );

    SDL_RenderFillRect(
        renderer,
        &rightRail
    );

    SDL_SetRenderDrawColor(
        renderer,
        110,
        112,
        114,
        255
    );

    int offset =
        static_cast<int>(
            guardRailOffset
        );

    for (
        int y = -80 + offset;
        y < screenHeight;
        y += 80
    ) {
        SDL_Rect leftPost = {
            leftRailX - 4,
            y,
            14,
            7
        };

        SDL_Rect rightPost = {
            rightRailX - 4,
            y,
            14,
            7
        };

        SDL_RenderFillRect(
            renderer,
            &leftPost
        );

        SDL_RenderFillRect(
            renderer,
            &rightPost
        );
    }
}

// ----------------------------------------------------
// HIGHWAY SIGNS
// ----------------------------------------------------

void DrivingScene::renderSigns() {
    const int signWidth =
        245;

    const int signHeight =
        72;

    int leftSignX =
        screenWidth / 2 -
        signWidth -
        150;

    int rightSignX =
        screenWidth / 2 +
        150;

    int signY = 55;

    // Sign poles.
    SDL_SetRenderDrawColor(
        renderer,
        125,
        125,
        125,
        255
    );

    SDL_Rect leftPole = {
        leftSignX +
        signWidth / 2 -
        4,
        signY +
        signHeight,
        8,
        75
    };

    SDL_Rect rightPole = {
        rightSignX +
        signWidth / 2 -
        4,
        signY +
        signHeight,
        8,
        75
    };

    SDL_RenderFillRect(
        renderer,
        &leftPole
    );

    SDL_RenderFillRect(
        renderer,
        &rightPole
    );

    // Green signs.
    SDL_SetRenderDrawColor(
        renderer,
        25,
        105,
        55,
        255
    );

    SDL_Rect leftSign = {
        leftSignX,
        signY,
        signWidth,
        signHeight
    };

    SDL_Rect rightSign = {
        rightSignX,
        signY,
        signWidth,
        signHeight
    };

    SDL_RenderFillRect(
        renderer,
        &leftSign
    );

    SDL_RenderFillRect(
        renderer,
        &rightSign
    );

    SDL_SetRenderDrawColor(
        renderer,
        245,
        245,
        245,
        255
    );

    SDL_RenderDrawRect(
        renderer,
        &leftSign
    );

    SDL_RenderDrawRect(
        renderer,
        &rightSign
    );

    textRenderer->renderFloatingText(
        renderer,
        "E470",
        leftSignX +
        signWidth / 2,
        signY + 14,
        signWidth - 20
    );

    textRenderer->renderFloatingText(
        renderer,
        "GUN CLUB ROAD",
        rightSignX +
        signWidth / 2,
        signY + 14,
        signWidth - 20
    );
}

// ----------------------------------------------------
// CAR
// ----------------------------------------------------

void DrivingScene::renderCar() {
    int drawX =
        static_cast<int>(
            carX
        );

    int drawY =
        static_cast<int>(
            carY
        );

    // Car shadow.
    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        0,
        0,
        0,
        85
    );

    SDL_Rect shadow = {
        drawX + 8,
        drawY + 10,
        carWidth,
        carHeight
    };

    SDL_RenderFillRect(
        renderer,
        &shadow
    );

    // Main body.
    SDL_SetRenderDrawColor(
        renderer,
        110,
        20,
        34,
        255
    );

    SDL_Rect body = {
        drawX,
        drawY,
        carWidth,
        carHeight
    };

    SDL_RenderFillRect(
        renderer,
        &body
    );

    // Hood.
    SDL_SetRenderDrawColor(
        renderer,
        135,
        25,
        42,
        255
    );

    SDL_Rect hood = {
        drawX + 7,
        drawY + 4,
        carWidth - 14,
        25
    };

    SDL_RenderFillRect(
        renderer,
        &hood
    );

    // Windshield.
    SDL_SetRenderDrawColor(
        renderer,
        35,
        48,
        58,
        255
    );

    SDL_Rect windshield = {
        drawX + 12,
        drawY + 31,
        carWidth - 24,
        34
    };

    SDL_RenderFillRect(
        renderer,
        &windshield
    );

    // Rear window.
    SDL_Rect rearWindow = {
        drawX + 14,
        drawY + 80,
        carWidth - 28,
        23
    };

    SDL_RenderFillRect(
        renderer,
        &rearWindow
    );

    // Headlights.
    SDL_SetRenderDrawColor(
        renderer,
        250,
        244,
        180,
        255
    );

    SDL_Rect leftLight = {
        drawX + 8,
        drawY + 3,
        16,
        7
    };

    SDL_Rect rightLight = {
        drawX +
        carWidth -
        24,
        drawY + 3,
        16,
        7
    };

    SDL_RenderFillRect(
        renderer,
        &leftLight
    );

    SDL_RenderFillRect(
        renderer,
        &rightLight
    );

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_NONE
    );
}

// ----------------------------------------------------
// INSTRUCTIONS
// ----------------------------------------------------

void DrivingScene::renderInstructions() {
    Uint32 elapsed =
        SDL_GetTicks() -
        sceneStartTime;

    if (routeCommitted) {
        std::string routeText;

        if (
            selectedRoute ==
            SceneType::E470
        ) {
            routeText =
                "E470 SELECTED";
        }
        else {
            routeText =
                "GUN CLUB ROAD SELECTED";
        }

        textRenderer->renderMessageBox(
            renderer,
            routeText,
            screenWidth,
            screenHeight - 100
        );

        return;
    }

    if (!controlsEnabled) {
        textRenderer->renderMessageBox(
            renderer,
            "Cruising...",
            screenWidth,
            35
        );

        return;
    }

    if (
        controlsEnabled &&
        elapsed <
        forkStartDelay
    ) {
        textRenderer->renderMessageBox(
            renderer,
            "Use LEFT and RIGHT to steer",
            screenWidth,
            35
        );

        return;
    }

    if (forkActive) {
        textRenderer->renderMessageBox(
            renderer,
            "Choose your route: E470 LEFT or Gun Club Road RIGHT",
            screenWidth,
            screenHeight - 90
        );
    }
}