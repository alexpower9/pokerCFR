//
// Created by Alex Power on 9/25/25.
//

#ifndef CPPTUTORIAL_PLAYER_H
#define CPPTUTORIAL_PLAYER_H
#include <iostream>
#include "BaseParticipant.h"
#include "Action.h"


class Player : public BaseParticipant {
public:
    Player(int startingStack, Position position) : BaseParticipant(startingStack, position) {};

    Action act(RoundContext& roundContext, unsigned int amountToCall) override;
    ~Player() override {
        std::cout << "Player is gone!" << std::endl;
    }

    [[nodiscard]] unsigned int getRaiseAmount(const RoundContext &roundContext) const;
};
#endif //CPPTUTORIAL_PLAYER_H