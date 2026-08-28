#ifndef TEXT_RENDERER_HPP
#define TEXT_RENDERER_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

// Handles font loading and drawing reusable text UI.
class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool loadFonts();

    void renderMessageBox(
        SDL_Renderer* renderer,
        const std::string& message,
        int screenWidth,
        int y
    );

    void renderCounter(
        SDL_Renderer* renderer,
        const std::string& counterText,
        int screenWidth,
        int y
    );

    void renderDialogueBox(
    SDL_Renderer* renderer,
    const std::string& message,
    int centerX,
    int topY,
    int maxWidth
);

    void renderSpeechBubble(
    SDL_Renderer* renderer,
    const std::string& message,
    int centerX,
    int topY,
    int maxWidth,
    bool tailPointsLeft
);

void renderFloatingText(
    SDL_Renderer* renderer,
    const std::string& message,
    int centerX,
    int topY,
    int maxWidth
);

    void shutdown();

private:
    TTF_Font* messageFont;
    TTF_Font* counterFont;

    TTF_Font* tryOpenFont(const std::string& path, int size);
};

#endif