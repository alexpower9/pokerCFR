#include <iostream>
#include "Card.h"
#include "Deck.h"
#include "Player.h"

int main() {

    Deck deck = Deck::create();
    deck.shuffle();
    auto alex = Player(1000, Player::Position::BB);
    alex.dealHand(deck);
    alex.seeHand();
    return 0;
}