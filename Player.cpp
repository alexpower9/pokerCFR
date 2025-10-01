//
// Created by Alex Power on 9/25/25.
//

#include "Player.h"
#include <iostream>

static constexpr std::array<Player::Move, 5> moves = {
    Player::Move::Call, Player::Move::Raise, Player::Move::Fold, Player::Move::Check, Player::Move::AllIn
};

void Player::dealHand(Deck &deck) {
    hand.push_back(deck.draw());
    hand.push_back(deck.draw());
}

void Player::seeHand() const {
    for (Card c: hand) {
        std::cout << c.toString() << " ";
    }
    std::cout << "\n";
}

//enum class Move {Call, Raise, Fold, Check, AllIn};
void Player::changeStack(int amount, int wonChips) {
    if (wonChips) {
        chips += amount;
    } else {
        chips -= amount;
    }
}
// street will most likely be changed to a different datatype
Player::Move Player::move() {
    std::cout << "It is your turn. Please enter the number for your move." << "\n";
    std::cout << "1. Call" << "\n";
    std::cout << "2. Raise" << "\n";
    std::cout << "3. Fold" << "\n";
    std::cout << "4. Check" << "\n";
    std::cout << "5. All-in" << std::endl;

    // should add some checking to see if this actually works
    int response;
    std::cin >> response;
    const Player::Move move {moves[response - 1]};

    return move;
}

void Player::wonHand(int amount) {
    changeStack(amount, 1);
}


int Player::call(int current_amount) {
    // check to be sure they have enough to actually call
    changeStack(current_amount, 0);
    return current_amount;
}

int Player::raise(int current_amount, int previous_raise_amount) {
    std::cout << "Enter the amount you would like to raise to: ";
    int new_total_bet;
    std::cin >> new_total_bet;

    // for now just include basic checks
    if ((new_total_bet - current_amount) < previous_raise_amount) {
        std::cout << "Raise amount was not valid, must be atleast " << previous_raise_amount + current_amount << '\n';
        return 0;
    } else {
        // bet is valid
        // assume we are going to return the amount they raised, not the new pot total
        changeStack(new_total_bet, 0);
        return new_total_bet;
    }
}

int Player::allIn() {
    const int total_amount = chips;
    changeStack(chips, 0);
    return total_amount;
}








