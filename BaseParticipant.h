//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_BASEPARTICIPANT_H
#define CPPTUTORIAL_BASEPARTICIPANT_H
#include "Action.h"
#include <vector>
#include "Card.h"
#include "Position.h"

struct RoundContext;
// this serves as the base call for Player and Bot
class BaseParticipant {
public:
    explicit BaseParticipant(int startingStack, Position position): stack(startingStack), position(position) {};

    virtual Action act(RoundContext &context, unsigned int amountToCall) = 0;
    virtual ~BaseParticipant() = default;

    void changeStack(unsigned int amount, bool won);
    void payBlinds(int bb, int sb);
    std::string getHand() const;

    void setPosition(Position position);
    Position getPosition() const { return position; };
    bool isFolded() const { return hasFolded; };
    void addCard(Card card);
    void resetHand();
    std::vector<Card> getCards() const;
    [[nodiscard]] inline unsigned int getStack() const { return stack; };

protected:
    unsigned int stack;
    std::vector<Card> hand;
    Position position;
    bool hasFolded = false;
};

#endif //CPPTUTORIAL_BASEPARTICIPANT_H