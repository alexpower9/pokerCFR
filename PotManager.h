//
// Created by Alex Power on 10/25/25.
//

#ifndef CPPTUTORIAL_POTMANAGER_H
#define CPPTUTORIAL_POTMANAGER_H
#include <unordered_map>
#include <iostream>
#include "BaseParticipant.h"

// This class will be used to track the entire hand contributions, and will help us
// decide sidepots at the end of each hand and how much a player should win/lose
struct Pot {
    unsigned int amount;
    std::vector<BaseParticipant*> eligiblePlayers;
    unsigned int capLevel; // just for debugging
};

class PotManager {

private:
    std::unordered_map<BaseParticipant*, unsigned int> totalContributions;
    std::vector<Pot> pots;

public:
    explicit PotManager(std::vector<BaseParticipant*> &players) {
        for (auto* player : players) {
            totalContributions[player] = 0;
        }
    }

    void addContribution(BaseParticipant* player, const unsigned int amount) {
        totalContributions[player] += amount;
    }

    unsigned int getContribution(BaseParticipant* &player) {
        return totalContributions[player];
    }

    [[nodiscard]] const std::vector<Pot> &getPots() const { return pots;};

    // we will call this when the betting round ends or someone goes all in
    void calculatePots(const std::vector<BaseParticipant*> &players);

    void printPots() const {
        for (size_t i = 0; i < pots.size(); i++) {
            std::cout << "Pot " << i << ": $" << pots[i].amount
                      << " (cap: " << pots[i].capLevel << ") - Eligible: ";
            for (auto* player : pots[i].eligiblePlayers) {
                std::cout << player->getName() << " ";
            }
            std::cout << std::endl;
        }
    }

    std::unordered_map<BaseParticipant*, unsigned int> getTotalPotContributions() {return totalContributions; };
};


#endif //CPPTUTORIAL_POTMANAGER_H