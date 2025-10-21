//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_POSITION_H
#define CPPTUTORIAL_POSITION_H
#include <string>

// redundant for now, but should help when we add more players to the game
// eventually
enum class Position {
    SB,
    BB,
    UTG
};

inline std::string positionToString(Position position) {
    switch (position) {
        case Position::SB: return "SB";
        case Position::BB: return "BB";
        case Position::UTG: return "UTG";
        default: return "Unknown position!";
    }
}

static constexpr std::array<Position, 3> ALL_POSITIONS = {
    Position::SB, Position::BB, Position::UTG
};

#endif //CPPTUTORIAL_POSITION_H