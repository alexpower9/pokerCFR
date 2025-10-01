//
// Created by Alex Power on 9/25/25.
//

#ifndef CPPTUTORIAL_PLAYER_H
#define CPPTUTORIAL_PLAYER_H
#include <array>
#include "Card.h"
#include "Deck.h"
class Player {

public:
    enum class Move {Call, Raise, Fold, Check, AllIn};
    enum class Position {BB, SB};

    explicit Player(const int starting_stack, const Position position): chips(starting_stack), position(position) {};

    static Move move();
    void seeHand() const;
    void dealHand(Deck& deck);
    void changeStack(int amount, int wonChips);
    void wonHand(int amount);
    int call(int current_amount);
    int raise(int current_amount, int previous_raise_amount);
    int allIn();

private:

    int chips; // how many chips the player has
    Position position;
    std::vector<Card> hand;
};

#endif //CPPTUTORIAL_PLAYER_H