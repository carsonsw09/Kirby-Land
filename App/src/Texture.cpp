#include "Texture.hpp"

#include <SDL2/SDL_image.h>
#include <iostream>
#include <queue>
#include <vector>
#include <cmath>

// Gets a pixel from a surface that has already been converted to RGBA32.
static Uint32 getPixel(SDL_Surface* surface, int x, int y) {
    Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;
    Uint32* pixels = reinterpret_cast<Uint32*>(row);
    return pixels[x];
}

// Sets a pixel on a surface that has already been converted to RGBA32.
static void setPixel(SDL_Surface* surface, int x, int y, Uint32 value) {
    Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;
    Uint32* pixels = reinterpret_cast<Uint32*>(row);
    pixels[x] = value;
}

// Determines whether a pixel looks like the unwanted background.
// This targets light gray / white background pixels like the ones
// that keep showing around your pajama sprite.
static bool isBackgroundPixel(SDL_PixelFormat* format, Uint32 pixel) {
    Uint8 r, g, b, a;
    SDL_GetRGBA(pixel, format, &r, &g, &b, &a);

    // Already transparent is definitely background.
    if (a == 0) {
        return true;
    }

    // Near-white / near-light-gray background detection.
    // This is designed to match the kind of flat light background
    // your sprite keeps arriving with.
    bool brightEnough = (r >= 235 && g >= 235 && b >= 235);
    bool lowColorDifference =
        (std::abs((int)r - (int)g) <= 10) &&
        (std::abs((int)g - (int)b) <= 10) &&
        (std::abs((int)r - (int)b) <= 10);

    return brightEnough && lowColorDifference;
}

// Flood-fills from the outer edges only, turning the detected
// background transparent. This is safer than deleting every white pixel
// because it only removes the connected outside background.
static void removeConnectedBackground(SDL_Surface* surface) {
    const int width = surface->w;
    const int height = surface->h;

    std::vector<bool> visited(width * height, false);
    std::queue<std::pair<int, int>> q;

    auto pushIfValid = [&](int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return;
        }

        int index = y * width + x;
        if (visited[index]) {
            return;
        }

        Uint32 pixel = getPixel(surface, x, y);
        if (!isBackgroundPixel(surface->format, pixel)) {
            return;
        }

        visited[index] = true;
        q.push({x, y});
    };

    // Start from all border pixels.
    for (int x = 0; x < width; x++) {
        pushIfValid(x, 0);
        pushIfValid(x, height - 1);
    }

    for (int y = 0; y < height; y++) {
        pushIfValid(0, y);
        pushIfValid(width - 1, y);
    }

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        Uint8 r, g, b, a;
        SDL_GetRGBA(getPixel(surface, x, y), surface->format, &r, &g, &b, &a);

        // Make this background pixel transparent.
        setPixel(surface, x, y, SDL_MapRGBA(surface->format, r, g, b, 0));

        pushIfValid(x + 1, y);
        pushIfValid(x - 1, y);
        pushIfValid(x, y + 1);
        pushIfValid(x, y - 1);
    }
}

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

    // Convert to RGBA32 so alpha handling is consistent.
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(
        loadedSurface,
        SDL_PIXELFORMAT_RGBA32,
        0
    );

    SDL_FreeSurface(loadedSurface);

    if (formattedSurface == nullptr) {
        std::cerr << "Could not convert image format: " << filePath << "\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        return false;
    }

    // Remove only the connected light background.
    removeConnectedBackground(formattedSurface);

    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, formattedSurface);

    if (newTexture == nullptr) {
        std::cerr << "Could not create texture from image: " << filePath << "\n";
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(formattedSurface);
        return false;
    }

    // This is critical for real transparency to display correctly.
    SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

    imageWidth = formattedSurface->w;
    imageHeight = formattedSurface->h;

    SDL_FreeSurface(formattedSurface);

    texture = newTexture;
    return true;
}

void Texture::render(SDL_Renderer* renderer, int x, int y, int width, int height) {
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destinationRect = { x, y, width, height };
    SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
}

void Texture::renderFlipped(
    SDL_Renderer* renderer,
    int x,
    int y,
    int width,
    int height,
    SDL_RendererFlip flip
) {
    if (texture == nullptr) {
        return;
    }

    SDL_Rect destinationRect = { x, y, width, height };

    SDL_RenderCopyEx(
        renderer,
        texture,
        nullptr,
        &destinationRect,
        0.0,
        nullptr,
        flip
    );
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