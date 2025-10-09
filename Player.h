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

    Player(const int starting_stack, const Position position): chips(starting_stack), position(position) {};
    static const std::unordered_map<Position, std::string> positionToString;

    void seeHand() const;
    void dealHand(Deck& deck);
    void changeStack(int amount, int wonChips);
    int getStack() const;
    void wonHand(int amount);
    int call(int current_amount);
    int raise(int current_amount, int previous_raise_amount);
    void changePosition();
    int allIn();
    Position getPosition() const;
    int payBlind(int bb, int sb);

    static Player::Move move(int amountToCall, int bb);


private:

    int chips; // how many chips the player has
    Position position;
    std::vector<Card> hand;
};

#endif //CPPTUTORIAL_PLAYER_H