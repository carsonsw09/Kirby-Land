#include "graphics/Texture.hpp"

#include <SDL2/SDL_image.h>
#include <cmath>
#include <iostream>
#include <queue>
#include <vector>

static Uint32 getPixel(SDL_Surface* surface, int x, int y) {
    Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;
    Uint32* pixels = reinterpret_cast<Uint32*>(row);
    return pixels[x];
}

static void setPixel(SDL_Surface* surface, int x, int y, Uint32 value) {
    Uint8* row = static_cast<Uint8*>(surface->pixels) + y * surface->pitch;
    Uint32* pixels = reinterpret_cast<Uint32*>(row);
    pixels[x] = value;
}

static bool isBackgroundPixel(SDL_PixelFormat* format, Uint32 pixel) {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;

    SDL_GetRGBA(pixel, format, &r, &g, &b, &a);

    if (a == 0) {
        return true;
    }

    bool brightEnough = r >= 235 && g >= 235 && b >= 235;

    bool lowColorDifference =
        std::abs(static_cast<int>(r) - static_cast<int>(g)) <= 10 &&
        std::abs(static_cast<int>(g) - static_cast<int>(b)) <= 10 &&
        std::abs(static_cast<int>(r) - static_cast<int>(b)) <= 10;

    return brightEnough && lowColorDifference;
}

static void removeConnectedBackground(SDL_Surface* surface) {
    int width = surface->w;
    int height = surface->h;

    std::vector<bool> visited(width * height, false);
    std::queue<std::pair<int, int>> pixelsToVisit;

    auto pushIfBackground = [&](int x, int y) {
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
        pixelsToVisit.push({x, y});
    };

    for (int x = 0; x < width; x++) {
        pushIfBackground(x, 0);
        pushIfBackground(x, height - 1);
    }

    for (int y = 0; y < height; y++) {
        pushIfBackground(0, y);
        pushIfBackground(width - 1, y);
    }

    while (!pixelsToVisit.empty()) {
        auto [x, y] = pixelsToVisit.front();
        pixelsToVisit.pop();

        Uint8 r;
        Uint8 g;
        Uint8 b;
        Uint8 a;

        SDL_GetRGBA(getPixel(surface, x, y), surface->format, &r, &g, &b, &a);
        setPixel(surface, x, y, SDL_MapRGBA(surface->format, r, g, b, 0));

        pushIfBackground(x + 1, y);
        pushIfBackground(x - 1, y);
        pushIfBackground(x, y + 1);
        pushIfBackground(x, y - 1);
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

    removeConnectedBackground(formattedSurface);

    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, formattedSurface);

    if (newTexture == nullptr) {
        std::cerr << "Could not create texture from image: " << filePath << "\n";
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(formattedSurface);
        return false;
    }

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

    SDL_Rect destinationRect = {
        x,
        y,
        width,
        height
    };

    SDL_RenderCopy(renderer, texture, nullptr, &destinationRect);
}

void Texture::renderCover(
    SDL_Renderer* renderer,
    int x,
    int y,
    int width,
    int height
) {
    if (texture == nullptr || imageWidth <= 0 || imageHeight <= 0) {
        return;
    }

    float imageAspect =
        static_cast<float>(imageWidth) /
        static_cast<float>(imageHeight);

    float destinationAspect =
        static_cast<float>(width) /
        static_cast<float>(height);

    SDL_Rect sourceRect = {
        0,
        0,
        imageWidth,
        imageHeight
    };

    // Image is wider than the destination.
    // Crop the left/right sides.
    if (imageAspect > destinationAspect) {
        int sourceWidth =
            static_cast<int>(imageHeight * destinationAspect);

        sourceRect.x = (imageWidth - sourceWidth) / 2;
        sourceRect.w = sourceWidth;
    }

    // Image is taller than the destination.
    // Crop the top/bottom.
    else if (imageAspect < destinationAspect) {
        int sourceHeight =
            static_cast<int>(imageWidth / destinationAspect);

        sourceRect.y = (imageHeight - sourceHeight) / 2;
        sourceRect.h = sourceHeight;
    }

    SDL_Rect destinationRect = {
        x,
        y,
        width,
        height
    };

    SDL_RenderCopy(
        renderer,
        texture,
        &sourceRect,
        &destinationRect
    );
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

    SDL_Rect destinationRect = {
        x,
        y,
        width,
        height
    };

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