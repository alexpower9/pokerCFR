//
// Created by Alex Power on 9/30/25.
//

#ifndef CPPTUTORIAL_BOT_H
#define CPPTUTORIAL_BOT_H
#include <iostream>
#include <utility>
#include "BaseParticipant.h"

// lets just first make the bot a calling station
class Bot : public BaseParticipant {
public:

    Bot(int startingStack, Position position, std::string name) : BaseParticipant(startingStack, position, std::move(name)) {};

    Action act(RoundContext &roundContext, unsigned int amountToCall) override;
    ~Bot() override {
        std::cout << "Bot is gone!" << std::endl;
    }

    // [[nodiscard]] unsigned int getRaiseAmount(const RoundContext &roundContext) const;

};


#endif //CPPTUTORIAL_BOT_H