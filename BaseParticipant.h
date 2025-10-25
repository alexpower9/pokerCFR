//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_BASEPARTICIPANT_H
#define CPPTUTORIAL_BASEPARTICIPANT_H
#include "Action.h"
#include <utility>
#include <vector>
#include "Card.h"
#include "Position.h"

struct RoundContext;
// this serves as the base call for Player and Bot
class BaseParticipant {
public:
    explicit BaseParticipant(int startingStack, Position position, std::string name) : stack(startingStack), position(position), name(std::move(name)) {};

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
    void unfold() { hasFolded = false; }
    std::vector<Card> getCards() const;
    std::string getName() {return name;};
    [[nodiscard]] inline unsigned int getStack() const { return stack; };

protected:
    unsigned int stack;
    std::vector<Card> hand;
    Position position;
    bool hasFolded = false;
    std::string name;
};

#endif //CPPTUTORIAL_BASEPARTICIPANT_H