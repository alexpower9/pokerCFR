//
// Created by Alex Power on 10/25/25.
//

#include "PotManager.h"
#include <set>
#include <iostream>
#include "RoundContext.h"

void PotManager::calculatePots(const std::vector<BaseParticipant *> &players) {
    pots.clear();

    // STEP 1: Get all unique all-in levels (sorted)
    std::set<unsigned int> allInLevels;

    for (auto &p: players) {
        if (p->hasWentAllIn()) {
            allInLevels.insert(totalContributions[p]);
            // std::cout << "  " << p->getName() << " is all-in at " << totalContributions[p] << std::endl;
        }
    }

    // std::cout << "Here all the all in levels for that round:\n";
    // for (const auto level : allInLevels) {
    //     std::cout << level << "\n";
    // }

    // Print total contributions for all players
    // std::cout << "\nTotal contributions:" << std::endl;
    // for (auto* p : players) {
    //     std::cout << "  " << p->getName() << ": " << totalContributions[p]
    //               << " (folded: " << p->isFolded() << ", all-in: " << p->hasWentAllIn() << ")" << std::endl;
    // }

    // STEP 2: Build pots for each all-in level
    unsigned int previousLevel = 0;

    for (unsigned int level : allInLevels) {
        // std::cout << "\nProcessing level " << level << " (previousLevel = " << previousLevel << "):" << std::endl;

        unsigned int potAmount = 0;
        std::vector<BaseParticipant*> eligiblePlayers;

        for (auto *player : players) {
            unsigned int contributionToThisPot =
                std::min(level, totalContributions[player]) - previousLevel;
            potAmount += contributionToThisPot;

            // std::cout << "  " << player->getName() << " contributes " << contributionToThisPot
            //           << " (total: " << totalContributions[player] << ")" << std::endl;

            // Player is eligible if they contributed to this level and haven't folded
            if (totalContributions[player] >= level && !player->isFolded()) {
                eligiblePlayers.push_back(player);
                // std::cout << "    -> ELIGIBLE" << std::endl;
            } else {
                // std::cout << "    -> not eligible (contrib=" << totalContributions[player]
                //           << ", level=" << level << ", folded=" << player->isFolded() << ")" << std::endl;
            }
        }

        // Add the pot if it has money in it
        if (potAmount > 0) {
            pots.push_back({potAmount, eligiblePlayers, level});
            // std::cout << "Created pot: $" << potAmount << " with " << eligiblePlayers.size() << " eligible players" << std::endl;
        }

        previousLevel = level;
    }

    // STEP 3: Create final pot for remaining chips
    // std::cout << "\nProcessing final pot (previousLevel = " << previousLevel << "):" << std::endl;

    unsigned int finalPotAmount = 0;
    std::vector<BaseParticipant*> finalEligible;

    for (auto* player : players) {
        if (totalContributions[player] > previousLevel) {
            unsigned int remainingContribution =
                totalContributions[player] - previousLevel;
            finalPotAmount += remainingContribution;

            // std::cout << "  " << player->getName() << " contributes " << remainingContribution << std::endl;

            if (!player->isFolded()) {
                finalEligible.push_back(player);
                // std::cout << "    -> ELIGIBLE" << std::endl;
            }
        }
    }

    if (finalPotAmount > 0) {
        pots.push_back({finalPotAmount, finalEligible, UINT_MAX});
        // std::cout << "Created final pot: $" << finalPotAmount << " with " << finalEligible.size() << " eligible players" << std::endl;
    }
}

