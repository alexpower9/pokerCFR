//
// Created by Alex Power on 10/9/25.
//
#include "BaseParticipant.h"
#include <iostream>
void BaseParticipant::changeStack(unsigned int amount, bool won) {
    if (won) {
        stack += amount;
    } else {
        stack -= amount;
    }
}

void BaseParticipant::payBlinds(int bb, int sb) {
    int amount = 0;
    switch (position) {
        case Position::BB: amount = bb; break;
        case Position::SB: amount = sb; break;
            default: break;
    }
    stack -= amount;
}

std::string BaseParticipant::getHand() const {
    std::string base = "";

    for (const auto &c : hand) {
        base += c.toString() + " ";
    }

    return base;
}

void BaseParticipant::setPosition(Position value) {
    position = value;
}

void BaseParticipant::addCard(Card card) {
    hand.push_back(card);
}

void BaseParticipant::resetHand() {
    hand.clear();
    hasFolded = false;
}

std::vector<Card> BaseParticipant::getCards() const {
    return hand;
}









