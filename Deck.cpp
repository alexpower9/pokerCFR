//
// Created by Alex Power on 9/25/25.
//

#include "Deck.h"
#include <algorithm>
#include <random>

Deck::Deck() = default;

Deck Deck::create() {
    Deck d;

    for (int i = 2; i <= 14; i++) {
        d.deck.push_back(Card::create(i, Card::Clubs));
        d.deck.push_back(Card::create(i, Card::Hearts));
        d.deck.push_back(Card::create(i, Card::Spades));
        d.deck.push_back(Card::create(i, Card::Diamonds));
    }

    return d;
}


void Deck::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(deck.begin(), deck.end(), g);
}

Card Deck::draw() {
    if (deck.empty()) {
        throw std::out_of_range("deck is empty!");
    }

    Card card = deck.back();
    deck.pop_back();
    return card;
}




