#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Texture.hpp"

#include <SDL2/SDL.h>
#include <string>

// Controls the player character movement, jumping, ducking, and drawing.
class Player {
public:
    Player();

    bool loadTexture(SDL_Renderer* renderer, const std::string& filePath);

    void setPosition(float startX, float startY);

    void handleInput(const SDL_Event& event);
    void update(int screenWidth, int groundY);
    void render(SDL_Renderer* renderer);

    int getWidth() const;
    int getHeight() const;

    float getX() const;
    bool isAtRightEdge(int screenWidth) const;

    SDL_Rect getCollisionBox() const;

private:
    Texture playerTexture;

    float x;
    float y;

    int width;
    int height;

    float velocityX;
    float velocityY;

    bool movingLeft;
    bool movingRight;
    bool jumping;
    bool ducking;

    bool facingRight;

    const float moveSpeed;
    const float jumpStrength;
    const float gravity;
};

#endif