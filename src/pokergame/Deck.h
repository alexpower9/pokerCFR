//
// Created by Alex Power on 9/25/25.
//

#ifndef CPPTUTORIAL_DECK_H
#define CPPTUTORIAL_DECK_H

#include "Card.h"
#include <vector> // a deck is just a vector at the end of the day

class Deck {
public:
    Deck();
    static Deck create();

    Card draw();
    void shuffle();

    // later we should add shuffle, draw

private:
    std::vector<Card> deck;
};

#endif //CPPTUTORIAL_DECK_H