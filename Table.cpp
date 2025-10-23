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
    //     std::cout << positionToString(p->getPosition()) << "\n";
    // }

    return order;
}


// we can actually just make this return an integer.
// if preflop, return the index of the player UTG if it exists, else just SB
// if not preflop, return the dealerIdx incremented by 1 (account for wrap around)
unsigned int Table::getAnchorIndex(Street street) const {
    if (street == Street::PreFlop) {
        for (unsigned int i = 0; i < players.size(); i++) {
            if (players[i]->getPosition() == Position::UTG) return i;
        }
        return 0; // this will be the small blind player, who is first to act on this street (Heads up)
    }

    unsigned int anchorIdx = dealerIdx + 1;
    if (anchorIdx == players.size()) anchorIdx = 0;

    return anchorIdx;

}


// we should call this right after construction, fixing the dealer button and assigning it accordingly
// this assigns the idx
// IF WE CHANGE INTIAL CONSTRUCTOR POSITIONS, THIS NEEDS TO CHANGE
// MAYBE WE SHOULD MAKE CONSTRUCTOR POSITIONS ARBITRARY BUT THIS IS POINTLESS
void Table::assignInitalDealer() {
    if (players.size() == 2) {
        dealerIdx = 0;
        return;
    }

    // we always begin positions in the game with SB, BB, and so on
    dealerIdx = players.size() - 1;
}


// so we will actually have the natural seat order define positions
// for example if we have two players, they swap idx each time, and we
// loop through the enum and reassign positions. Since we also have the button
// we should keep track of that too
void Table::movePositions() {
    dealerIdx = (dealerIdx + 1) % players.size();
    // now we should loop through the
    for (unsigned i = 0; i < players.size(); i++) {
        // so we can make all positions relative to the dealer
        unsigned const relative = (i - dealerIdx + players.size()) % players.size();
        players[i].get()->setPosition(ALL_POSITIONS[relative]); // CHECK THIS!
    }
}

unsigned int Table::getNumberOfActivePlayers() const {
    unsigned int total = 0;

    for (auto &p : players) {
        if (!p->isFolded()) total += 1;
    }

    return total;
}


// new deck, shuffle, deal everyone two cards
void Table::beginRound(int bb, int sb, RoundContext &roundContext, std::vector<BaseParticipant*> &playersInOrder) {
    deck = Deck::create();
    deck.shuffle();

    // first clear the hands and ensure everyone is not folded
    for (auto &p:players) {
        p->resetHand();
    }

    for (auto &p : players) {
        p->addCard(deck.draw());
        p->addCard(deck.draw());
    }

    for (int i = 0; i < players.size(); i++) {
        if (playersInOrder[i]->getPosition() == Position::BB) {
            roundContext.roundContributions[i] += bb;
            playersInOrder[i]->changeStack(bb, 0);
        } else if (playersInOrder[i]->getPosition() == Position::SB) {
            roundContext.roundContributions[i] += sb;
            playersInOrder[i]->changeStack(sb, 0);
        }
    }
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








