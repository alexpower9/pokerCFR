//
// Created by Alex Power on 10/9/25.
//

#ifndef CPPTUTORIAL_TABLE_H
#define CPPTUTORIAL_TABLE_H
#include <vector>
#include "BaseParticipant.h"
#include "Deck.h"
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
    BaseParticipant* next(unsigned& idx) const;
    std::vector<BaseParticipant*> getPlayersInOrder(Street street) const;
    void movePositions();
    void beginRound(int bb, int sb, RoundContext &roundContext, std::vector<BaseParticipant *> &playersInOrder);
    // void postBlinds(int bb, int sb) const;
    unsigned int getNumberOfActivePlayers() const; // this might be able to get deleted
    [[nodiscard]] const std::vector<std::unique_ptr<BaseParticipant>>& getPlayers() const {
        return players;
    }
    std::vector<Card> getFullHand(std::vector<Card> &hand) const; // this essentially joins a player hand with community
    // cards, sorts, and returns

    std::string boardToString() const;
    void flop();
    void turn();
    void river();

private:
    std::vector<std::unique_ptr<BaseParticipant>> players;
    std::vector<Card> communityCards;
    Deck deck;
    unsigned dealerIdx;
};

#endif //CPPTUTORIAL_TABLE_H