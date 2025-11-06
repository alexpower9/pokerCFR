//
// Created by Alex Power on 9/25/25.
//

#ifndef CPPTUTORIAL_CARD_H
#define CPPTUTORIAL_CARD_H
#include <string>

class Card {

public:
    enum Suit {Hearts, Diamonds, Clubs, Spades};

    static Card create(const int rank, const Suit suit) {
        if (rank < 2 || rank > 14) throw std::invalid_argument("Invalid rank");
        return Card{rank, suit};
    }

    // we need to sort cards for least to greatest
    bool operator<(const Card &other) const {
        return rank > other.rank;
    }

    [[nodiscard]] std::string toString() const;

    Suit getSuit() const;
    int getRank() const;

private:
    int rank;
    Suit suit;

    Card(const int rank, const Suit suit) : rank(rank), suit(suit) {}
};

#endif //CPPTUTORIAL_CARD_H