#include "CollectibleItem.hpp"

CollectibleItem::CollectibleItem()
    : x(0),
      y(0),
      width(70),
      height(70),
      collected(false) {
}

bool CollectibleItem::loadTexture(SDL_Renderer* renderer, const std::string& filePath) {
    return texture.loadFromFile(renderer, filePath);
}

void CollectibleItem::setPosition(int startX, int startY) {
    x = startX;
    y = startY;
}

void CollectibleItem::setSize(int itemWidth, int itemHeight) {
    width = itemWidth;
    height = itemHeight;
}

void CollectibleItem::render(SDL_Renderer* renderer) {
    if (collected) {
        return;
    }

    texture.render(renderer, x, y, width, height);
}

SDL_Rect CollectibleItem::getCollisionBox() const {
    SDL_Rect box = {
        x,
        y,
        width,
        height
    };

    return box;
}

bool CollectibleItem::isCollected() const {
    return collected;
}

void CollectibleItem::collect() {
    collected = true;
}

void CollectibleItem::reset() {
    collected = false;
}