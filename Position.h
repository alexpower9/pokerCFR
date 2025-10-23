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
    UTG,
    MP
};

inline std::string positionToString(Position position) {
    switch (position) {
        case Position::SB: return "SB";
        case Position::BB: return "BB";
        case Position::UTG: return "UTG";
        case Position::MP: return "MP";
        default: return "Unknown position!";
    }
}

static constexpr std::array<Position, 4> ALL_POSITIONS = {
    Position::SB, Position::BB, Position::UTG, Position::MP
};

#endif //CPPTUTORIAL_POSITION_H