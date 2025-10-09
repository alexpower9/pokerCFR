#include <iostream>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Game.h"

// useful for debugging
u_int32_t extract(const u_int32_t ranking, const int high, const int low) {
    const uint32_t mask = (1u << (high - low + 1)) - 1;
    return (ranking >> low) & mask;
}

int main() {
    Game game {10, 5, 1000};
    game.runGame();

    return 0;
}