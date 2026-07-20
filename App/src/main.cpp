#include "core/Game.hpp"
#include "core/SceneType.hpp"

#include <iostream>
#include <string>

// Converts a terminal scene name into the matching scene type.
static SceneType parseSceneName(const std::string& sceneName) {
    if (sceneName == "start") {
        return SceneType::Start;
    }

    if (sceneName == "bedroom") {
        return SceneType::Bedroom;
    }

    if (sceneName == "home") {
        return SceneType::Home;
    }

    std::cerr << "Unknown scene: " << sceneName << "\n";
    std::cerr << "Valid scenes are: start, bedroom, home\n";
    std::cerr << "Starting from the normal start screen instead.\n";

    return SceneType::Start;
}

// Entry point for Kirby Land.
int main(int argc, char* argv[]) {
    SceneType startingScene = SceneType::Start;

    for (int i = 1; i < argc; i++) {
        std::string argument = argv[i];

        if (argument == "--scene") {
            if (i + 1 < argc) {
                startingScene = parseSceneName(argv[i + 1]);
                i++;
            }
            else {
                std::cerr << "Missing scene name after --scene.\n";
                std::cerr << "Valid scenes are: start, bedroom, home\n";
            }
        }
    }

    Game game(startingScene);

    if (!game.initialize()) {
        std::cerr << "Failed to start Kirby Land.\n";
        return 1;
    }

    game.run();

    return 0;
}