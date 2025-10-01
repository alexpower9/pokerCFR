//
// Created by Alex Power on 9/30/25.
//
#include "Card.h"
#include "Deck.h"
#include "Bot.h"

#include <iostream>


// static constexpr std::array<Bot::Move, 5> moves = {
//     Bot::Move::Call, Bot::Move::Raise, Bot::Move::Fold, Bot::Move::Check, Bot::Move::AllIn
// };

void Bot::seeHand() const {
    for (auto c: hand) {
        std::cout << c.toString() << " ";
    }
    std::cout << "\n";
}


void Bot::changeStack(int amount, int wonChips) {
    if (wonChips) {
        chips += amount;
    } else {
        chips -= amount;
    }

}

void Bot::wonHand(int amount) {
    changeStack(amount, 1);
}

// for now, just make it call everytime
Bot::Move Bot::move() {
    return Bot::Move::Call;
}

void Bot::dealHand(Deck &deck) {
    hand.push_back(deck.draw());
    hand.push_back(deck.draw());
}





