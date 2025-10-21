//
// Created by Alex Power on 9/30/25.
//
#include "Bot.h"
#include "Action.h"
#include <iostream>

#include "RoundContext.h"

Action Bot::act(RoundContext &roundContext, unsigned int amountToCall) {
    // this will always just be a call!
    Action action;

    action.raiseAmount = 0;
    action.move = Move::Call;
    action.betAmount = amountToCall;

    if (stack >= amountToCall) {
        changeStack(amountToCall, 0);
    }


    return action;
}









