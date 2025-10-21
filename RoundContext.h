//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_ROUNDCONTEXT_H
#define CPPTUTORIAL_ROUNDCONTEXT_H
#include "Street.h"
#include "Move.h"
#include "BaseParticipant.h"

struct RoundContext {
    unsigned int potSize;
    unsigned bb;
    unsigned prevRaise; // this will hold the amount previously raised, not the total
    Street street;
    Move lastMove;
    int endCode = 0; // if endCode == 1, everyone folded besides 1 person. If endCode == 2, we went to showdown
    std::unordered_map<unsigned int, unsigned int> roundContributions; // use this to keep track of how much
    // someone has bet per round

    void createRoundContributions(const std::vector<BaseParticipant*> &players) {
        // so I need to take a vector of players and turn it into a std map,
        // where idx is the key
        for (unsigned int i = 0; i < players.size(); ++i) {
            roundContributions[i] = 0;
        }
    }

    // use this to check the amount the player has to call
    // where totalContribution is the amt the player must contribute to stay in the hand
    // this should just be the the difference in the amount the most recent valid player has put in
    unsigned int amountToCall(unsigned int idx, const std::vector<BaseParticipant*> &players) {
        const unsigned int amountContributed = roundContributions[idx];

        unsigned int prevPlayer = idx;

        while (true) {
            if (prevPlayer == 0) {
                prevPlayer = players.size() - 1;
            } else {
                prevPlayer--;
            }

            if (!players[prevPlayer]->isFolded()) {
                break;
            }
        }

        // std::cout << "Amount this player has contributed: " << amountContributed << "\n";
        // std::cout << "Amount previous player has contributed: " << roundContributions[prevPlayer] << "\n";
        return roundContributions[prevPlayer] - amountContributed;

    }
};

#endif //CPPTUTORIAL_ROUNDCONTEXT_H