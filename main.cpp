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

    // std::vector<Card> hand = {Card::create(14, Card::Spades), Card::create(13, Card::Spades)};
    // std::vector<Card> communityCards = {
    //     Card::create(12, Card::Spades),
    //     Card::create(11, Card::Spades),
    //     Card::create(10, Card::Spades),
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
    // std::cout << "Hand rank is " << decodeHandRank(handRank) << std::endl;

    return 0;
}