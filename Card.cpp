//
// Created by Alex Power on 9/25/25.
//

#include "Card.h"
#include <sstream>

std::string Card::toString() const {
    std::ostringstream oss;

    if (rank >= 2 && rank <= 10) {
        oss << rank;
    } else {
        switch (rank) {
            case 11: oss << "J"; break;
            case 12: oss << "Q"; break;
            case 13: oss << "K"; break;
            case 14: oss << "A"; break;
            default: ;
        }
    }

    switch (suit) {
        default:;
        case Hearts: oss << "h"; break;
        case Diamonds: oss << "d"; break;
        case Spades: oss << "s"; break;
        case Clubs: oss << "c"; break;
    }

    return oss.str();

}
