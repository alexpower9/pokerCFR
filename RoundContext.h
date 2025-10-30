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
    std::unordered_map<BaseParticipant*, unsigned int> roundContributions; // use this to keep track of how much
    // someone has bet per round

    void createRoundContributions(const std::vector<BaseParticipant*> &players) {
        // so I need to take a vector of players and turn it into a std map,
        // where idx is the key
        // for (unsigned int i = 0; i < players.size(); ++i) {
        //     roundContributions[i] = 0;
        // }
        for (const auto &p: players) {
            roundContributions[p] = 0;
        }
    }
    // use this to check the amount the player has to call
    // where totalContribution is the amt the player must contribute to stay in the hand
    // this should just be the the difference in the amount the most recent valid player has put in
    unsigned int amountToCall(BaseParticipant* &player) {
        const unsigned int amountContributed = roundContributions[player];

        unsigned int maxContributed = 0;

        // for (int i = 0; i < players.size(); i++) {
        //     maxContributed = std::max(maxContributed, roundContributions[i]);
        // }

        for (const auto &[fst, snd]:roundContributions) {
            maxContributed = std::max(maxContributed, roundContributions[fst]);
        }

        if (const unsigned int playerStack = player->getStack(); maxContributed >= playerStack) return playerStack;
        return maxContributed - amountContributed;
    }
};

#endif //CPPTUTORIAL_ROUNDCONTEXT_H