//
// Created by Alex Power on 9/25/25.
//
#include "Player.h"
#include <iostream>

static constexpr std::array<Player::Move, 5> moves = {
    Player::Move::Call, Player::Move::Raise, Player::Move::Fold, Player::Move::Check, Player::Move::AllIn
};

const std::unordered_map<Player::Position, std::string> Player::positionToString = {
    {Player::Position::BB, "BB"},
    {Player::Position::SB, "SB"},
};

void Player::changePosition() {
    if (position == Player::Position::BB) {
        position = Player::Position::SB;
    } else {
        position = Player::Position::BB;
    }
}

int Player::payBlind(int bb, int sb) {
    if (position == Player::Position::BB) {
        changeStack(bb, 0);
        return bb;
    } else {
        changeStack(sb, 0);
        return sb;
    }
}

int Player::getStack() const {
    return chips;
}

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

Player::Position Player::getPosition() const {
    return position;
}

//enum class Move {Call, Raise, Fold, Check, AllIn};
void Player::changeStack(int amount, int wonChips) {
    if (wonChips) {
        chips += amount;
    } else {
        chips -= amount;
    }
}

// if call amountToCall is 0, then we know we can check, so we should not show
// an option to fold
// and if we can fold and return -1, lets say we folded
Player::Move Player::move(const int amountToCall, const int bb) {
    std::cout << "It is your turn!\n";

    if (amountToCall == 0) {
        std::cout << "1. Bet (" << bb << ")\n" << "2. Check\n" << "3. All-in";
    } else {
        std::cout << "1. Call (" << amountToCall << ")\n" << "2. Raise\n" << "3. All-in" << "4. Fold\n";
    }

    // we need to add some mechanism for input validation
    // now if else disasterclass
    while (true) {
        int response = -1;
        std::cin >> response;
        Player::Move move;

        if (amountToCall == 0) {
            switch (response) {
                case 1:
                    move = Player::Move::Call;
                    return move;
                case 2:
                    move = Player::Move::Check;
                    return move;
                case 3:
                    move = Player::Move::AllIn;
                    return move;
                default:
                    std::cout << "Please enter a valid option, 1-3 inclusive.\n";
                    break;
            }
        } else {
            switch (response) {
                case 1:
                    move =  Player::Move::Call;
                    return move;
                case 2:
                    move =  Player::Move::Raise;
                    return move;
                case 3:
                    move = Player::Move::AllIn;
                    return move;
                case 4:
                    move = Player::Move::Fold;
                    return move;
                default:
                    std::cout << "Please enter a valid option, 1-4 inclusive.\n";
                    break;
            }
        }
    }

    // if the code is fucked, just jam as default
    return Player::Move::AllIn;
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








