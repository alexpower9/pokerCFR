//
// Created by Alex Power on 9/30/25.
//
#include <iostream>
#include <tuple>
#include "Game.h"

void Game::startGame() {

}

// maybe return 3 for a tie
int Game::evaluateHands(std::vector<Card> hand1, std::vector<Card> hand2) {
    Game::HandRanking handRank1 = classifyHand(hand1);
}

Game::HandRanking Game::classifyHand(std::vector<Card> hand) {
    //enum class HandRanking {HighCard, OnePair, TwoPair, Trips, Straight, Flush, FullHouse, Quads, StraightFlush};
    //it would be cool to export hand rankings as some bit sequence, where say 4 bits represent the hand type and then
    // the other bits represent the rank


}


void Game::seeBoard() const {
    for (Card card: communityCards) {
        std::cout << card.toString() << " ";
    }
    std::cout << "\n";
}
