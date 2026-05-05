#include "Game.hpp"

#include <iostream>

// Starts Kirby Land.
int main() {
    Game game;

    if (!game.initialize()) {
        std::cerr << "Failed to start Kirby Land.\n";
        return 1;
    }

    game.run();

    return 0;
}