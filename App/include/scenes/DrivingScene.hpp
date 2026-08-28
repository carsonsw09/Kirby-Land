#ifndef DRIVING_SCENE_HPP
#define DRIVING_SCENE_HPP

#include "core/Scene.hpp"
#include "graphics/TextRenderer.hpp"

#include <SDL2/SDL.h>

#include <random>
#include <vector>

// Short driving transition where the player chooses
// between E470 and Gun Club Road.
class DrivingScene : public Scene {
public:
    DrivingScene(
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
    enum class RoadsideObjectType {
        Tree,
        Bush,
        Rock,
        SpeedSign,
        WarningSign,
        UtilityPole
    };

    struct RoadsideObject {
        RoadsideObjectType type;

        float x;
        float y;

        float scale;

        bool leftSide;
    };

    void renderRoad();

    void renderStraightRoad();
    void renderForkRoad();

    void renderLaneMarkers();
    void renderRoadShoulders();
    void renderRoadTexture();

    void renderCar();

    void renderSigns();
    void renderInstructions();

    void initializeRoadsideObjects();
    void updateRoadsideObjects(float deltaTime);

    void resetRoadsideObject(
        RoadsideObject& object,
        bool placeAboveScreen
    );

    void renderRoadsideScenery();
    void renderRoadsideObject(
        const RoadsideObject& object
    );

    void drawTree(
        int x,
        int y,
        float scale
    );

    void drawBush(
        int x,
        int y,
        float scale
    );

    void drawRock(
        int x,
        int y,
        float scale
    );

    void drawSpeedSign(
        int x,
        int y,
        float scale
    );

    void drawWarningSign(
        int x,
        int y,
        float scale
    );

    void drawUtilityPole(
        int x,
        int y,
        float scale
    );

    void drawGuardRails();

    void commitRoute(SceneType route);

    SDL_Renderer* renderer;
    TextRenderer* textRenderer;

    int screenWidth;
    int screenHeight;

    float carX;
    float carY;

    int carWidth;
    int carHeight;

    float carMoveSpeed;

    bool movingLeft;
    bool movingRight;

    bool controlsEnabled;
    bool forkActive;
    bool routeCommitted;

    SceneType selectedRoute;

    Uint32 sceneStartTime;
    Uint32 lastUpdateTime;
    Uint32 routeCommitTime;

    const Uint32 controlUnlockDelay;
    const Uint32 forkStartDelay;
    const Uint32 forkAnimationDuration;
    const Uint32 routeDisplayDuration;

    float forkProgress;

    float laneScrollOffset;
    float roadTextureOffset;
    float guardRailOffset;

    const float roadScrollSpeed;

    int roadWidth;

    std::vector<RoadsideObject> roadsideObjects;

    std::mt19937 randomGenerator;
};

#endif