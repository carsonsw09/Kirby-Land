#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <string>

// Handles loading, rendering, flipping, and freeing image textures.
class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(SDL_Renderer* renderer, const std::string& filePath);

    void render(SDL_Renderer* renderer, int x, int y, int width, int height);

    void renderFlipped(
        SDL_Renderer* renderer,
        int x,
        int y,
        int width,
        int height,
        SDL_RendererFlip flip
    );

    void renderCover(
    SDL_Renderer* renderer,
    int x,
    int y,
    int width,
    int height
);

    void free();

    int getWidth() const;
    int getHeight() const;

private:
    SDL_Texture* texture;
    int imageWidth;
    int imageHeight;
};

#endif