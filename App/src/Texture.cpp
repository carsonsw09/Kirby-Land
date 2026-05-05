#include "Texture.hpp"

#include <SDL2/SDL_image.h>
#include <iostream>

Texture::Texture()
    : texture(nullptr),
      imageWidth(0),
      imageHeight(0) {
}

Texture::~Texture() {
    free();
}

bool Texture::loadFromFile(SDL_Renderer* renderer, const std::string& filePath) {
    free();

    SDL_Surface* loadedSurface = IMG_Load(filePath.c_str());

    if (loadedSurface == nullptr) {
        std::cerr << "Could not load image: " << filePath << "\n";
        std::cerr << "IMG_Load Error: " << IMG_GetError() << "\n";
        return false;
    }

    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);

    if (newTexture == nullptr) {
        std::cerr << "Could not create texture from image: " << filePath << "\n";
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(loadedSurface);
        return false;
    }

    imageWidth = loadedSurface->w;
    imageHeight = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);

    texture = newTexture;
    return true;
}

void Texture::render(SDL_Renderer* renderer, int x, int y, int width, int height) {
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destinationRect = {
        x,
        y,
        width,
        height
    };

    SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
}

void Texture::free() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
        imageWidth = 0;
        imageHeight = 0;
    }
}

int Texture::getWidth() const {
    return imageWidth;
}

int Texture::getHeight() const {
    return imageHeight;
}