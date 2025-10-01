//
// Created by Alex Power on 9/30/25.
//

#ifndef CPPTUTORIAL_BOT_H
#define CPPTUTORIAL_BOT_H
#include "Card.h"
#include "Deck.h"
#include <vector>

// lets just first make the bot a calling station
class Bot {
public:
    enum class Move {Call, Raise, Fold, Check, AllIn};
    enum class Position {BB, SB};

    static Move move();
    void dealHand(Deck &deck);
    void seeHand() const;
    void changeStack(int amount, int wonChips);
    void wonHand(int amount);




    // constructor
    explicit Bot(const int startingStack, Position position): chips(startingStack), position(position) {};

private:
    int chips;
    std::vector<Card> hand;
    Position position;
};


#endif //CPPTUTORIAL_BOT_H