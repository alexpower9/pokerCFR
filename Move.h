//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_MOVE_H
#define CPPTUTORIAL_MOVE_H

#include <string>
enum class Move {
    Fold,
    Check,
    Bet,
    Call,
    Raise,
    AllIn,
    NO_MOVE
};

// const is fucking useless here
inline std::string moveToString(const Move move) {
    switch (move) {
        case Move::Fold: return "Fold";
        case Move::Check: return "Check";
        case Move::Bet: return "Bet";
        case Move::Call: return "Call";
        case Move::Raise: return "Raise";
        case Move::AllIn: return "All In";
        case::Move::NO_MOVE: return "";
        default:  return "Unknown move";
    }
}

constexpr static std::array<Move, 7> MOVES_ARRAY = {
    Move::Fold,
    Move::Check,
    Move::Bet,
    Move::Call,
    Move::Raise,
    Move::AllIn,
    Move::NO_MOVE
};

#endif //CPPTUTORIAL_MOVE_H