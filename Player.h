//
// Created by Alex Power on 9/25/25.
//

#ifndef CPPTUTORIAL_PLAYER_H
#define CPPTUTORIAL_PLAYER_H
#include <iostream>
#include <utility>
#include "BaseParticipant.h"
#include "Action.h"


class Player : public BaseParticipant {
public:
    Player(int startingStack, Position position, std::string name) : BaseParticipant(startingStack, position, std::move(name)) {};

    Action act(RoundContext& roundContext, unsigned int amountToCall) override;
    ~Player() override {
        std::cout << getName() << " is gone!" << std::endl;
    }

    [[nodiscard]] unsigned int getBetAmount(const RoundContext &roundContext) const;
};
#endif //CPPTUTORIAL_PLAYER_H