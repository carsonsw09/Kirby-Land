#include "Player.hpp"

Player::Player()
    : x(100.0f),
      y(0.0f),
      width(220),
      height(300),
      velocityX(0.0f),
      velocityY(0.0f),
      movingLeft(false),
      movingRight(false),
      jumping(false),
      ducking(false),
      facingRight(true),
      moveSpeed(2.5f),
      jumpStrength(-18.0f),
      gravity(0.8f) {
}

bool Player::loadTexture(SDL_Renderer* renderer, const std::string& filePath) {
    return playerTexture.loadFromFile(renderer, filePath);
}

void Player::setPosition(float startX, float startY) {
    x = startX;
    y = startY;
}

void Player::handleInput(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
            case SDLK_a:
            case SDLK_LEFT:
                movingLeft = true;
                facingRight = false;
                break;

            case SDLK_d:
            case SDLK_RIGHT:
                movingRight = true;
                facingRight = true;
                break;

            case SDLK_s:
            case SDLK_DOWN:
                ducking = true;
                break;

            case SDLK_SPACE:
                if (!jumping) {
                    velocityY = jumpStrength;
                    jumping = true;
                }
                break;

            case SDLK_w:
            case SDLK_UP:
                // Up currently does nothing.
                break;

            default:
                break;
        }
    }

    if (event.type == SDL_KEYUP && event.key.repeat == 0) {
        switch (event.key.keysym.sym) {
            case SDLK_a:
            case SDLK_LEFT:
                movingLeft = false;
                break;

            case SDLK_d:
            case SDLK_RIGHT:
                movingRight = false;
                break;

            case SDLK_s:
            case SDLK_DOWN:
                ducking = false;
                break;

            default:
                break;
        }
    }
}

void Player::update(int screenWidth, int groundY) {
    velocityX = 0.0f;

    if (movingLeft) {
        velocityX -= moveSpeed;
    }

    if (movingRight) {
        velocityX += moveSpeed;
    }

    x += velocityX;

    if (x < 0.0f) {
        x = 0.0f;
    }

    if (x + width > screenWidth) {
        x = static_cast<float>(screenWidth - width);
    }

    y += velocityY;

    if (jumping) {
        velocityY += gravity;
    }

    if (y + height >= groundY) {
        y = static_cast<float>(groundY - height);
        velocityY = 0.0f;
        jumping = false;
    }
}

void Player::render(SDL_Renderer* renderer) {
    int drawX = static_cast<int>(x);
    int drawY = static_cast<int>(y);
    int drawWidth = width;
    int drawHeight = height;

    if (ducking) {
        drawHeight = height / 2;
        drawY = static_cast<int>(y + height / 2);
    }

    SDL_RendererFlip flip = facingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    playerTexture.renderFlipped(
        renderer,
        drawX,
        drawY,
        drawWidth,
        drawHeight,
        flip
    );
}

int Player::getWidth() const {
    return width;
}

int Player::getHeight() const {
    return height;
}