//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_TABLE_H
#define CPPTUTORIAL_TABLE_H
#include <vector>
#include "BaseParticipant.h"
#include "Deck.h"
#include <iostream>
enum class Street;
// we can use the table to manage the players and the positions of everything
// set as setting player positions and such as it is much easier this way
class Table {
public:
    explicit Table(std::vector<std::unique_ptr<BaseParticipant>> &&players): players(std::move(players)),
    dealerIdx(0) {

    };

    // now we can actually advance the next player int
    // in the game class from here
    std::vector<BaseParticipant*> getPlayersInOrder(Street street) const;
    bool movePositions();
    void removeBustedPlayers();
    void assignPositionsFromDealer();
    void beginRound();
    // void postBlinds(int bb, int sb) const;
    unsigned int getNumberOfActivePlayers() const; // this might be able to get deleted
    [[nodiscard]] const std::vector<std::unique_ptr<BaseParticipant>>& getPlayers() const {
        return players;
    }
    std::vector<Card> getFullHand(std::vector<Card> &hand) const; // this essentially joins a player hand with community
    [[nodiscard]] unsigned int getAnchorIndex(Street street) const;
    void assignInitialDealer() const;
    unsigned int getDealerIdx() const { return dealerIdx; };
    std::vector<Position> getPositionsForNumPlayers(unsigned int numPlayers) const;


    // this will be for awarding the chips at the end
    std::vector<BaseParticipant*> determineWinners(std::vector<BaseParticipant*> eligiblePlayers);


    std::string boardToString() const;
    void flop();
    void turn();
    void river();

private:
    std::vector<std::unique_ptr<BaseParticipant>> players;
    std::vector<Card> communityCards;
    Deck deck;
    mutable unsigned int dealerIdx;
};

#endif //CPPTUTORIAL_TABLE_H