//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_ACTION_H
#define CPPTUTORIAL_ACTION_H
#include "Move.h"
struct Action {
    Move move;
    unsigned int betAmount;
    unsigned int raiseAmount;

    [[nodiscard]] inline std::string actionToString() const {
        return "Move:" + moveToString(move) + " Amount:" + std::to_string(betAmount);
    }
};




#endif //CPPTUTORIAL_ACTION_H