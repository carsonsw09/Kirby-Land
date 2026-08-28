#ifndef E470_SCENE_HPP
#define E470_SCENE_HPP

#include "core/Scene.hpp"
#include "graphics/TextRenderer.hpp"

#include <SDL2/SDL.h>

#include <random>
#include <vector>

class E470Scene : public Scene {
public:
    E470Scene(
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
    struct TrafficCar {
        float x;
        float y;

        int width;
        int height;

        int lane;

        float speed;

        Uint8 red;
        Uint8 green;
        Uint8 blue;

        bool active;
    };

    void spawnTrafficWave();
    void spawnCar(
        int lane,
        float yOffset
    );

    void updateTraffic(
        float deltaTime
    );

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

    void renderEnvironment();
    void renderRoad();
    void renderLaneMarkers();
    void renderRoadside();
    void renderTraffic();
    void renderTrafficCar(
        const TrafficCar& car
    );
    void renderPlayerCar();
    void renderHUD();
    void renderCompleteScreen();

    float getLaneX(
        int lane,
        int objectWidth
    ) const;

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
    float sceneryScrollOffset;

    const float roadSpeed;

    std::vector<TrafficCar> traffic;

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