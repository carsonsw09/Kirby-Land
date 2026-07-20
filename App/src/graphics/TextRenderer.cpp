#include "graphics/TextRenderer.hpp"

#include <iostream>

TextRenderer::TextRenderer()
    : messageFont(nullptr),
      counterFont(nullptr) {
}

TextRenderer::~TextRenderer() {
    shutdown();
}

TTF_Font* TextRenderer::tryOpenFont(const std::string& path, int size) {
    return TTF_OpenFont(path.c_str(), size);
}

bool TextRenderer::loadFonts() {
    messageFont = tryOpenFont(
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf",
        30
    );

    if (messageFont == nullptr) {
        messageFont = tryOpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf",
            30
        );
    }

    if (messageFont == nullptr) {
        messageFont = tryOpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            30
        );
    }

    if (messageFont == nullptr) {
        messageFont = tryOpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            30
        );
    }

    if (messageFont == nullptr) {
        std::cerr << "Could not load message font.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return false;
    }

    counterFont = tryOpenFont(
        "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf",
        44
    );

    if (counterFont == nullptr) {
        counterFont = tryOpenFont(
            "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
            44
        );
    }

    if (counterFont == nullptr) {
        std::cerr << "Could not load counter font.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return false;
    }

    return true;
}

void TextRenderer::renderMessageBox(
    SDL_Renderer* renderer,
    const std::string& message,
    int screenWidth,
    int y
) {
    if (messageFont == nullptr) {
        return;
    }

    SDL_Color textColor = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(
        messageFont,
        message.c_str(),
        textColor,
        screenWidth - 80
    );

    if (textSurface == nullptr) {
        std::cerr << "Could not create message text surface.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == nullptr) {
        std::cerr << "Could not create message text texture.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {
        (screenWidth - textWidth) / 2,
        y,
        textWidth,
        textHeight
    };

    SDL_Rect boxRect = {
        textRect.x - 20,
        textRect.y - 12,
        textRect.w + 40,
        textRect.h + 24
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 130);
    SDL_RenderFillRect(renderer, &boxRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}

void TextRenderer::renderCounter(
    SDL_Renderer* renderer,
    const std::string& counterText,
    int screenWidth,
    int y
) {
    if (counterFont == nullptr) {
        return;
    }

    SDL_Color textColor = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* textSurface = TTF_RenderText_Blended(
        counterFont,
        counterText.c_str(),
        textColor
    );

    if (textSurface == nullptr) {
        std::cerr << "Could not create counter text surface.\n";
        std::cerr << "TTF Error: " << TTF_GetError() << "\n";
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == nullptr) {
        std::cerr << "Could not create counter text texture.\n";
        std::cerr << "SDL Error: " << SDL_GetError() << "\n";
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {
        screenWidth - textWidth - 35,
        y,
        textWidth,
        textHeight
    };

    SDL_Rect boxRect = {
        textRect.x - 16,
        textRect.y - 10,
        textRect.w + 32,
        textRect.h + 20
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, &boxRect);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}

void TextRenderer::renderFloatingText(
    SDL_Renderer* renderer,
    const std::string& message,
    int centerX,
    int topY,
    int maxWidth
) {
    if (messageFont == nullptr) {
        return;
    }

    SDL_Color textColor = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(
        messageFont,
        message.c_str(),
        textColor,
        maxWidth
    );

    if (textSurface == nullptr) {
        SDL_Log("Could not create floating text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == nullptr) {
        SDL_Log("Could not create floating text texture: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {
        centerX - textWidth / 2,
        topY,
        textWidth,
        textHeight
    };

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}

void TextRenderer::shutdown() {
    if (messageFont != nullptr) {
        TTF_CloseFont(messageFont);
        messageFont = nullptr;
    }

    if (counterFont != nullptr) {
        TTF_CloseFont(counterFont);
        counterFont = nullptr;
    }
}

void TextRenderer::renderDialogueBox(
    SDL_Renderer* renderer,
    const std::string& message,
    int centerX,
    int topY,
    int maxWidth
) {
    if (counterFont == nullptr) {
        return;
    }

    SDL_Color textColor = {
        255,
        255,
        255,
        255
    };

    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(
    counterFont,
    message.c_str(),
    textColor,
    maxWidth
);

    if (textSurface == nullptr) {
        SDL_Log("Could not create dialogue text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (textTexture == nullptr) {
        SDL_Log("Could not create dialogue text texture: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {
        centerX - textWidth / 2,
        topY,
        textWidth,
        textHeight
    };

    SDL_Rect boxRect = {
        textRect.x - 18,
        textRect.y - 12,
        textRect.w + 36,
        textRect.h + 24
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
    SDL_RenderFillRect(renderer, &boxRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 210);
    SDL_RenderDrawRect(renderer, &boxRect);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
}