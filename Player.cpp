//
// Created by Alex Power on 9/25/25.
//
#include "Player.h"
#include <iostream>
#include "Action.h"
#include "RoundContext.h"
// moves in order: fold, check, bet, call, Raise, Allin
// Recall that round context has
// callAmount, potSize, bb, street, lastMove

// action as move, amount
Action Player::act(RoundContext &roundContext, unsigned int amountToCall) {
    const std::string prevMove = (roundContext.lastMove == Move::NO_MOVE) ? std::string{"You are first to act"} : "Last move was a " + moveToString(roundContext.lastMove);
    std::cout << getName() << "\n";
    std::cout << prevMove <<
        ". Pot is currently: " << roundContext.potSize <<
            ". Your stack is: " << stack
        << ". Your position: " << positionToString(position)
        << "\n";


    Action responseAction;
    responseAction.betAmount = 0;
    responseAction.raiseAmount = 0;

    unsigned response = 0;

    while (true) {
        if (amountToCall == 0) {
            std::cout << "1. Check 2. Bet (" << roundContext.bb << ") 3. Raise 4. All-in\n";
        } else {
            std::cout << "1. Fold 2. Call (" << amountToCall << ") 3. Raise 4. All-in\n";
        }

        std::cin >> response;


        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (amountToCall == 0) {
            switch (response) {
                case 1:
                    responseAction.raiseAmount = 0;
                    responseAction.move = Move::Check;
                    break;
                case 2:
                    responseAction.raiseAmount = roundContext.bb;
                    responseAction.betAmount = roundContext.bb;
                    responseAction.move = Move::Bet;
                    changeStack(responseAction.betAmount, 0);



                    break;
                case 3: {
                    // we should treat raise amount as just the new total
                    const unsigned int newBet = getBetAmount(roundContext);
                    responseAction.raiseAmount = newBet - amountToCall;
                    responseAction.betAmount = newBet;
                    changeStack(responseAction.betAmount, 0);
                    responseAction.move = Move::Raise;
                    break;
                }
                case 4:
                    responseAction.raiseAmount = stack - amountToCall;
                    // responseAction.betAmount = responseAction.raiseAmount;
                    responseAction.betAmount = stack;
                    changeStack(responseAction.betAmount, 0);
                    responseAction.move = Move::AllIn;

                    break;
                default:
                    std::cout << "Please enter a valid move.\n";
                    continue; // loop again
            }
        } else {
            switch (response) {
                case 1:
                    responseAction.move = Move::Fold;
                    hasFolded = true;
                    break;
                case 2: {
                    // we need to check if the player has enough to call here
                    responseAction.move = Move::Call;

                    if (amountToCall > stack) amountToCall = stack;
                    responseAction.betAmount = amountToCall;
                    changeStack(responseAction.betAmount, 0);
                    break;
                }
                case 3: {
                    // we should treat raise amount as just the new total
                    const unsigned int newBet = getBetAmount(roundContext);
                    responseAction.raiseAmount = newBet - amountToCall;
                    responseAction.betAmount = newBet;
                    changeStack(responseAction.betAmount, 0);
                    responseAction.move = Move::Raise;
                    break;
                }
                case 4:
                    responseAction.raiseAmount = stack - amountToCall;
                    responseAction.betAmount = stack;
                    // responseAction.betAmount = responseAction.raiseAmount;
                    changeStack(responseAction.betAmount, 0);
                    responseAction.move = Move::AllIn;
                    break;
                default:
                    std::cout << "Please enter a valid move.\n";
                    continue;
            }
        }

        break;
    }

    // cheap fix to return AllIn when the amount the player has called leaves their stack to 0
    if (getStack() == 0) {
        responseAction.move = Move::AllIn;
        isAllIn = true;
        std::cout << "Player has jammed\n";
    }

    return responseAction;

}

// recall this is just the amount they are raising to
unsigned int Player::getBetAmount(const RoundContext &roundContext) const {
    int amount = 0;

    while (true) {
        std::cout << "Enter the amount you would like to bet. The previous raise was " << roundContext.prevRaise << "\n";
        std::cin >> amount;
        const unsigned int raiseAmount = amount - roundContext.prevRaise;
        if (raiseAmount < roundContext.bb || raiseAmount < roundContext.prevRaise || raiseAmount > stack) {
            std::cout << "Invalid amount! Please try again.";
        } else {
            break;
        }
    }

    return amount;
}







