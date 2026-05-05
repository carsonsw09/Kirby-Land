#ifndef COLLECTIBLE_ITEM_HPP
#define COLLECTIBLE_ITEM_HPP

#include "Texture.hpp"

#include <SDL2/SDL.h>
#include <string>

// Represents an item that can appear in the room and be collected.
class CollectibleItem {
public:
    CollectibleItem();

    bool loadTexture(SDL_Renderer* renderer, const std::string& filePath);

    void setPosition(int startX, int startY);
    void setSize(int itemWidth, int itemHeight);

    void render(SDL_Renderer* renderer);

    SDL_Rect getCollisionBox() const;

    bool isCollected() const;
    void collect();
    void reset();

private:
    Texture texture;

    int x;
    int y;

    int width;
    int height;

    bool collected;
};

#endif