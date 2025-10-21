//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_STREET_H
#define CPPTUTORIAL_STREET_H
#include <string>

enum class Street {
    PreFlop,
    Flop,
    Turn,
    River
};

inline std::string streetToString(Street street) {
    switch (street) {
        case Street::PreFlop: return "Preflop";
        case Street::Flop: return "Flop";
        case Street::Turn: return "Turn";
        case Street::River: return "River";
            default: return"Error occurred!";
    }
}

static constexpr std::array<Street, 4> ALL_STREETS = {
    Street::PreFlop, Street::Flop, Street::Turn, Street::River
};
#endif //CPPTUTORIAL_STREET_H