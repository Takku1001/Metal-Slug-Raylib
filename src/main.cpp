#include "Game.h"
#include <iostream>

int main() {
    try {
        Game metalSlugGame;
        metalSlugGame.Initialize();
        metalSlugGame.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}