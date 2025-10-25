#include <iostream>
#include "Card.h"
#include "Deck.h"
#include "Player.h"
#include "Game.h"

//imports for the test function
#include <cstdint>
#include <string>
#include <vector>
#include <sstream>



int main() {
    Game game {10, 5, 1000};
    game.runGame();

    // std::vector<Card> hand = {Card::create(14, Card::Hearts), Card::create(2, Card::Spades)};
    // std::vector<Card> communityCards = {
    //     Card::create(3, Card::Spades),
    //     Card::create(4, Card::Spades),
    //     Card::create(5, Card::Spades),
    //     Card::create(9, Card::Diamonds),
    //     Card::create(11, Card::Hearts)
    // };
    //
    // communityCards.insert(communityCards.end(), hand.begin(), hand.end());
    // std::sort(communityCards.begin(), communityCards.end());
    //
    // for (auto const &p:communityCards) {
    //     std::cout << p.toString() << "\n";
    // }
    //
    // uint32_t handRank = Game::classifyHand(communityCards);
    //
    // std::cout << "Hand rank is " << Game::decodeHandRank(handRank) << std::endl;

    return 0;
}