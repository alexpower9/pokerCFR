//
// Created by Alex Power on 10/9/25.
//
#include "Table.h"
#include "BaseParticipant.h"
#include "Position.h"
#include <sstream>
#include "Street.h"
#include <iostream>

#include "RoundContext.h"

// we can return the order the players need to be in quite simply
std::vector<BaseParticipant*> Table::getPlayersInOrder(const Street street) const {

    std::vector<BaseParticipant*> order;
    // so we just find the idx of that player, and build the order starting from them
    const unsigned int startIdx = getAnchorIndex(street);
    // std::cout << "The startIdx is: " << startIdx << "\n";
    // now we build the order beginning from that player
    for (unsigned int i = 0; i < players.size(); ++i) {
        const unsigned int idx = (startIdx + i) % players.size();

        // kinda dumb from the IDE
        if (BaseParticipant* player = players[idx].get(); !player->isFolded()) {
            order.push_back(player);
        }
    }

    //just check order
    // for (auto &p : order) {
    //     std::cout << positionToString(p->getPosition()) << " ";
    // }


    std::cout << "\n";
    return order;
}


// we can actually just make this return an integer.
// if preflop, return the index of the player UTG if it exists, else just SB
// if not preflop, return the dealerIdx incremented by 1 (account for wrap around)
// If we begin with the dealer idx off by 1, then increment it each time, this should be fine
unsigned int Table::getAnchorIndex(Street street) const {
    // std::cout << "The dealer idx for this round is: " << dealerIdx << "\n";
    if (street == Street::PreFlop) {
        // if its heads up, we anchor aread the BB
        if (players.size() == 2) {
            for (unsigned int i = 0; i < players.size(); i++) {
                if (players[i]->getPosition() == Position::BB) {
                    return i;
                }
            }
        }

        // if more than 2 players, just take the player after the BB
        for (unsigned int i = 0; i < players.size(); i++) {
            if (players[i]->getPosition() == Position::BB) {
                return (i + 1) % players.size();
            }
        }
    }

    // now we can just make the player after the dealer the first to act
    unsigned int anchorIdx = (dealerIdx + 1) % players.size();
    return anchorIdx;

}


// we should call this right after construction, fixing the dealer button and assigning it accordingly
// this assigns the idx
// IF WE CHANGE INTIAL CONSTRUCTOR POSITIONS, THIS NEEDS TO CHANGE
void Table::assignInitialDealer() const {
    if (players.size() == 2) {
        dealerIdx = 0;
        return;
    }

    // we always begin positions in the game with SB, BB, and so on
    dealerIdx = players.size() - 1; // so if we have SB, BB, UTG, then BB will be dealer,
}


// so we will actually have the natural seat order define positions
// for example if we have two players, they swap idx each time, and we
// loop through the enum and reassign positions. Since we also have the button
// we should keep track of that too
void Table::movePositions() {
    removeBustedPlayers();

    if (players.size() < 2) {
        // we need to implement logic for when the game ends
        return;
    }

    dealerIdx = (dealerIdx + 1) % players.size();
    assignPositionsFromDealer();
    // unfold everyone at the start of the round
    for (const auto &p:players) {
        p->unfold();
        p->unAllIn();
    }

}

void Table::removeBustedPlayers() {
    size_t removedBeforeDealer = 0;
    for (size_t i = 0; i < dealerIdx && i < players.size(); ++i) {
        if (players[i]->getStack() == 0) {
            removedBeforeDealer++;
        }
    }

    // Check if dealer is busted
    bool dealerBusted = (dealerIdx < players.size() && players[dealerIdx]->getStack() == 0);

    // Remove busted players
    players.erase(
        std::remove_if(players.begin(), players.end(),
            [](const auto& p) { return p->getStack() == 0; }),
        players.end()
    );

    // Adjust dealer index
    dealerIdx -= removedBeforeDealer;
    if (dealerBusted) {
        // adjust by one less since we will increment in movePositions
        if (dealerIdx > 0) dealerIdx--;
    }

    if (dealerIdx >= players.size() && !players.empty()) {
        dealerIdx = 0;
    }
}

void Table::assignPositionsFromDealer() {
    size_t numPlayers = players.size();

    if (numPlayers == 2) {
        players[dealerIdx]->setPosition(Position::SB);
        players[(dealerIdx + 1) % numPlayers]->setPosition(Position::BB);
    } else if (numPlayers == 3) {
        players[dealerIdx]->setPosition(Position::BUTTON);
        players[(dealerIdx + 1) % numPlayers]->setPosition(Position::SB);
        players[(dealerIdx + 2) % numPlayers]->setPosition(Position::BB);
    } else if (numPlayers == 4){
        players[dealerIdx]->setPosition(Position::BUTTON);
        players[(dealerIdx + 1) % numPlayers]->setPosition(Position::SB);
        players[(dealerIdx + 2) % numPlayers]->setPosition(Position::BB);
        players[(dealerIdx + 3) % numPlayers]->setPosition(Position::UTG);

    } else if (numPlayers == 5) {
        players[dealerIdx]->setPosition(Position::BUTTON);
        players[(dealerIdx + 1) % numPlayers]->setPosition(Position::SB);
        players[(dealerIdx + 2) % numPlayers]->setPosition(Position::BB);
        players[(dealerIdx + 3) % numPlayers]->setPosition(Position::UTG);
        players[(dealerIdx + 4) % numPlayers]->setPosition(Position::MP);
    } else {
        players[dealerIdx]->setPosition(Position::BUTTON);
        players[(dealerIdx + 1) % numPlayers]->setPosition(Position::SB);
        players[(dealerIdx + 2) % numPlayers]->setPosition(Position::BB);
        players[(dealerIdx + 3) % numPlayers]->setPosition(Position::UTG);
        players[(dealerIdx + 4) % numPlayers]->setPosition(Position::MP);
        players[(dealerIdx + 5) % numPlayers]->setPosition(Position::CO);
    }
}



unsigned int Table::getNumberOfActivePlayers() const {
    unsigned int total = 0;

    for (auto &p : players) {
        if (!p->isFolded() && p->getStack() != 0) total += 1;
    }

    return total;
}


// new deck, shuffle, deal everyone two cards
void Table::beginRound() {
    deck = Deck::create();
    deck.shuffle();

    for (auto &p : players) {
        p->resetHand();
        p->addCard(deck.draw());
        p->addCard(deck.draw());
    }

    communityCards.clear();
}

// we should deprecate this when we get the chance
// void Table::postBlinds(int bb, int sb) const {
//     for (auto &p: players) {
//         p->payBlinds(bb, sb);
//     }
// }

std::string Table::boardToString() const {
    std::ostringstream oss;

    if (!communityCards.empty()) {
        for (auto &c: communityCards) {
            oss << c.toString() << " ";
        }
    } else {
        oss << "Pre-flop, nothing on the board.";
    }

    return oss.str();
}

void Table::flop() {
    for (int i = 0; i < 3; i++) {
        communityCards.push_back(deck.draw());
    }
}

void Table::turn() {
    communityCards.push_back(deck.draw());
}

void Table::river() {
    communityCards.push_back(deck.draw());
}

std::vector<Card> Table::getFullHand(std::vector<Card> &hand) const {
    std::vector<Card> hand1 = hand;
    hand1.insert(hand1.end(), communityCards.begin(), communityCards.end());

    std::sort(hand1.begin(), hand1.end());

    return hand1;
}



