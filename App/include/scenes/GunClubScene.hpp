#ifndef GUN_CLUB_SCENE_HPP
#define GUN_CLUB_SCENE_HPP

#include "core/Scene.hpp"
#include "graphics/TextRenderer.hpp"

#include <SDL2/SDL.h>

#include <random>
#include <vector>

class GunClubScene : public Scene {
public:
    GunClubScene(
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
    enum class HazardType {
        Bulldozer,
        Cones,
        Pipes
    };

    struct ConstructionHazard {
        HazardType type;

        float x;
        float y;

        int width;
        int height;

        int lane;

        float speed;

        bool active;
    };

    struct Pothole {
        float x;
        float y;

        int width;
        int height;
    };

    void spawnHazardWave();
    void spawnHazard(
        int lane,
        HazardType type
    );

    void updateHazards(
        float deltaTime
    );

    void updatePotholes(
        float deltaTime
    );

    void createPotholes();

    void checkCollisions();

    bool intersects(
        float ax,
        float ay,
        int aw,
        int ah,
        float bx,
        float by,
        int bw,
        int bh
    ) const;

    float getLaneX(
        int lane,
        int objectWidth
    ) const;

    void renderEnvironment();
    void renderRoad();
    void renderRoadDamage();
    void renderLaneMarkers();
    void renderConstructionScenery();

    void renderHazards();

    void drawBulldozer(
        const ConstructionHazard& hazard
    );

    void drawCones(
        const ConstructionHazard& hazard
    );

    void drawPipes(
        const ConstructionHazard& hazard
    );

    void renderPotholes();

    void renderPlayerCar();
    void renderHUD();
    void renderCompleteScreen();

    SDL_Renderer* renderer;
    TextRenderer* textRenderer;

    int screenWidth;
    int screenHeight;

    int roadWidth;
    int roadX;

    float playerX;
    float playerY;

    int playerWidth;
    int playerHeight;

    float playerSpeed;

    bool movingLeft;
    bool movingRight;

    float roadScrollOffset;
    float damageScrollOffset;

    const float roadSpeed;

    std::vector<ConstructionHazard>
        hazards;

    std::vector<Pothole>
        potholes;

    Uint32 sceneStartTime;
    Uint32 lastUpdateTime;
    Uint32 lastSpawnTime;

    const Uint32 levelDuration;

    bool levelComplete;

    int hits;

    Uint32 hitMessageStart;
    const Uint32 hitMessageDuration;

    std::mt19937 randomGenerator;
};

#endif