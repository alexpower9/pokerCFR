//
// Created by Alex Power on 9/30/25.
//
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "Bot.h"
#include "RoundContext.h"
#include "Table.h"
#include <vector>
#include "Move.h"


const std::unordered_map<u_int32_t, std::string> Game::rankToString = {
    {0, "HighCard"},
    {1, "Pair"},
    {2, "TwoPair"},
    {3, "Trips"},
    {4, "Straight"},
    {5, "Flush"},
    {6, "FullHouse"},
    {7, "Quads"},
    {8, "StraightFlush"}
};


void Game::reset_deck() {
    deck = Deck::create();
    deck.shuffle();
}

void Game::runGame() {
   // we will try this again
    // should go something like this:
    // reset_deck()
    // deal hands
    // post blinds
    // playStreet(Preflop)
    // playStreet(Flop)... and so on
    // determine winner
    // update stacks, run it back
    std::cout << "New hand is beginning!\n";
    // while (true) {
    std::vector<BaseParticipant*> initialOrder = table.getPlayersInOrder(Street::PreFlop);
    RoundContext roundContext = newHandContext(initialOrder);
    table.beginRound(bigBlind, smallBlind, roundContext, initialOrder); // new deck, deal 2 cards


    for (int i = 0; i < ALL_STREETS.size(); i++) {
        const Street street = ALL_STREETS[i];
        std::cout << "Current street is " << streetToString(street) << "\n";
        playStreet(street, roundContext);
        if (roundContext.endCode == 1 || roundContext.endCode == 2) break;

        if (ALL_STREETS[i] != Street::River) {
            roundContext = createRoundContextForNextRound(ALL_STREETS[i + 1], roundContext.potSize, roundContext.endCode);
        }
    }

    // we are going to need the roundContext endCode (but not really)
    handleWinner(roundContext);


    // }
}



// maybe return 3 for a tie
int Game::evaluateHands(const std::vector<Card> &hand1, const std::vector<Card> &hand2) {
    std::vector<Card> combined1 = hand1;
    std::vector<Card> combined2 = hand2;
    //
    // combined1.insert(combined1.end(), communityCards.begin(), communityCards.end());
    // combined2.insert(combined2.end(), communityCards.begin(), communityCards.end());

    std::sort(combined1.begin(), combined1.end());
    std::sort(combined2.begin(), combined2.end());


    const u_int32_t handRank1 = classifyHand(hand1);
    const u_int32_t handRank2 = classifyHand(hand2);

    if (handRank1 > handRank2) {
        return 1;
    } else if (handRank2 > handRank1) {
        return 2;
    } else {
        return 3;
    }
}

// reminder that this will be given the full 7 Card hand, already sorted.
u_int32_t Game::classifyHand(const std::vector<Card> &hand) {
    // we will actually use an integer to do this:
    // first 4 bits will be hand ranking
    // then primary rank (5 bits)
    // then secondary rank (4 bits)
    // then kicker (4 bits)
    // then kicker2 (4 bits)
    // then kicker (4 bits)

    // mappings for hand strength
    // straight flush: 1000
    // quads: 0111
    // full house: 0110
    // flush: 0101
    //straight: 0100
    // trips: 0011
    // two pair: 0010
    // one pair: 0001
    // high card: 0000
    u_int32_t ranking = 0;

    bool isFlush = false;
    bool isStraight = false;
    bool quads = false;
    bool fullHouse = false;
    bool trips = false;
    bool twoPair = false;
    bool pair = false;
    bool highCard = false;


    std::unordered_map<Card::Suit, int> freqTable = {
        {Card::Suit::Hearts, 0},
        {Card::Suit::Diamonds, 0},
        {Card::Suit::Clubs, 0},
        {Card::Suit::Spades, 0}};
    // from 0 index, it goes hearts, diamonds, clubs, spades
    // check flushes
    for (const Card &c : hand) {
        freqTable[c.getSuit()]++;
    }

    // use this vector to check straight flushes
    std::vector<Card> flushCards;
    Card::Suit flushSuit;
    // if any of these are 5, we have flush
    for (const auto &pair : freqTable) {
        if (pair.second >= 5) {
            flushSuit = pair.first;
            isFlush = true;
        }
    }

    if (isFlush) {
        for (const auto &c : hand) {
            if (c.getSuit() == flushSuit) {
                flushCards.push_back(c);
            }
        }

        int run = 1;
        for (int i = 0; i < flushCards.size() - 1; i++) {
            if (flushCards[i].getRank() - flushCards[i + 1].getRank() == 1) {
                run++;

                if (run == 5) {
                    // now we can actually return a straight flush
                    // RETURN HERE
                    // return std::make_tuple(Game::HandRanking::StraightFlush, flushCards[i-4].getRank());
                    ranking |= (8 & 0xF) << 28;
                    ranking |= (flushCards[i - 4].getRank() & 0x1F) << 23;
                    return ranking;
                } else if (run == 4 && hasAce(hand) && hand[i + 1].getRank() == 2) {
                    // wheel straight
                    ranking |= (8 & 0xF) << 28;
                    ranking |= (5 & 0x1F) << 23;
                    return ranking;
                }
            } else {
                run = 1;
            }
        }

    }

    // now quads
    // quad kicker
    int quadKicker = 0;
    int quadRank;
    for (int i = 0; i < 4; i++) {
        if (hand[i].getRank() == hand[i + 1].getRank() && hand[i + 1].getRank() == hand[i + 2].getRank()
           && hand[i + 2].getRank() == hand[i + 3].getRank()) {
            quads = true;
            quadRank = hand[i].getRank();
            // return tuple for quads here
            ranking |= (7 & 0xF) << 28;
            ranking |= (quadRank & 0x1F) << 23;
        }
    }

    if (quads) {
        for (const auto &c : hand) {
            if (c.getRank() != quadRank) {
                ranking |= (c.getRank() & 0x1F) << 18;
            }
        }
        return ranking;
    }

    //trips
    int tripsRank;
    for (int i = 0; i < 5; i++) {
        if (hand[i].getRank() == hand[i + 1].getRank() && hand[i + 1].getRank() == hand[i + 2].getRank()) {
            trips = true;
            tripsRank = hand[i].getRank();
           }
    }

    // check two pair and single pair at once
    int pairRank1 = 0;
    int pairRank2 = 0;
    for (int i = 0; i < hand.size() - 1; i++) {
        // pair found, check if we already found a pair though
        if (hand[i].getRank() == hand[i + 1].getRank()) {
            if (pairRank1 == 0) {
                pairRank1 = hand[i].getRank();
            } else {
                // this must be the second pair
                pairRank2 = hand[i].getRank();
            }
        }
    }

    // now we check for full house
    if (trips && pairRank1 != 0 && pairRank1 != tripsRank) {
        ranking |= (6 & 0xF) << 28;
        ranking |= (tripsRank & 0x1F) << 23;
        ranking |= (pairRank1 & 0x1F) << 18; // no clue if this is actually a thing
        // we still need the other high card here

        return ranking;
    }

    // now flush
    if (isFlush) {
        ranking |= (5 & 0xF) << 28;
        ranking |= (flushCards[0].getRank() & 0x1F) << 23;
        return ranking;
    }

    // check for straight
    int run = 1;
    for (int i = 0; i < hand.size() - 1; i++) {


        if (hand[i].getRank() - hand[i + 1].getRank() == 1 ) {
            run++;
            if (run == 5) {
                // also return the rank here
                ranking |= (4 & 0xF) << 28;
                ranking |= (hand[i - 4].getRank() & 0x1F) << 23;
                return ranking;
            } else if (run == 4 && hasAce(hand) && hand[i + 1].getRank() == 2) {
                // wheel ace
                ranking |= (4 & 0xF) << 28;
                ranking |= (5 & 0x1F) << 23;
                return ranking;
            }
        } else {
            run = 1;
        }
    }

    if (trips) {
        std::vector<int> tripsKickers;

        for (const auto &c: hand) {
            if (c.getRank() != tripsRank) {
                tripsKickers.push_back(c.getRank());
            }
        }

        ranking |= (3 & 0xF) << 28;
        ranking |= (tripsKickers[0] & 0x1F) << 23;
        ranking |= (tripsKickers[1] & 0x1F) << 18;
        return ranking;
    }


    if (pairRank1 != 0 && pairRank2 != 0) {
        // two pair
        int kicker = 0;
        for (const auto &c : hand) {
            if (c.getRank() != pairRank1 && c.getRank() != pairRank2) {
                kicker = c.getRank();

                ranking |= (2 & 0xF) << 28;           // category
                ranking |= (pairRank1 & 0x1F) << 23;  // high pair
                ranking |= (pairRank2 & 0x1F) << 18;  // low pair
                ranking |= (kicker & 0x1F) << 13;     // kicker

                return ranking;
            }
        }
    }

    if (pairRank1 != 0) {
        // one pair
        std::vector<int> kickers;

        for (const auto &c: hand) {
            if (c.getRank() != pairRank1) {
                kickers.push_back(c.getRank());
            }
        }
        ranking |= (1 & 0xF) << 28;
        ranking |= (pairRank1 & 0x1F) << 23;
        ranking |= (kickers[0] & 0x1F) << 18;
        ranking |= (kickers[1] & 0x1F) << 13;
        ranking |= (kickers[2] & 0x1F) << 8;

        return ranking;
    }

    // finally, just return high card
    // bad code
    std::vector<int> kickers;
    for (int i = 1; i < hand.size(); i++) {
        kickers.push_back(hand[i].getRank());
    }

    ranking |= (hand[0].getRank() & 0x1F) << 23;
    ranking |= (kickers[0] & 0x1F) << 18;
    ranking |= (kickers[1] & 0x1F) << 13;
    ranking |= (kickers[2] & 0x1F) << 8;
    ranking |= (kickers[3] & 0x1F) << 3;

    return ranking;

}

// void Game::seeBoard() const {
//     for (Card card: communityCards) {
//         std::cout << card.toString() << " ";
//     }
//     std::cout << "\n";
// }

// check for an ace
bool Game::hasAce(const std::vector<Card> &hand) {
    if (hand[0].getRank() == 14) {
        return true;
    }

    return false;
}

RoundContext Game::newHandContext(const std::vector<BaseParticipant*> &players) const {
    RoundContext newContext;
    newContext.bb = bigBlind;
    newContext.lastMove = Move::Call; // COULD BE BAD
    newContext.potSize = bigBlind + smallBlind;
    newContext.street = Street::PreFlop;
    newContext.prevRaise = 0;
    newContext.createRoundContributions(players);
    return newContext;
}

// we are going to need to carry over some info from the previous street
RoundContext Game::createRoundContextForNextRound(const Street street, const unsigned int potSize, int endCode) const {
    RoundContext newContext;
    newContext.bb = bigBlind;
    newContext.lastMove = Move::NO_MOVE; // COULD BE BAD
    newContext.potSize = potSize;
    newContext.street = street;
    newContext.prevRaise = 0;
    newContext.endCode = endCode;
    return newContext;
}

//moves are in this order: Fold, Check, Bet, Call, Raise, All-in
//RoundContext has: callAmount, potSize, bb, prevRaise, street, lastMove
void Game::updateRoundContext(RoundContext &roundContext, Action action, const unsigned int playerIdx) const{
    switch (action.move) {
        case Move::Fold: roundContext.lastMove = Move::Fold; break;

        case Move::Call: roundContext.lastMove = Move::Call;
            roundContext.potSize += action.betAmount; roundContext.roundContributions[playerIdx] += action.betAmount;
            break;

        case Move::Check: roundContext.lastMove = Move::Check; break;

        case Move::Bet: roundContext.lastMove = Move::Bet;
            roundContext.potSize += action.betAmount; roundContext.roundContributions[playerIdx] += action.betAmount;
            break;

        case Move::Raise: roundContext.lastMove = Move::Raise;
            roundContext.potSize += action.betAmount;
            roundContext.roundContributions[playerIdx] += action.betAmount;
            roundContext.prevRaise = action.betAmount; break;

        case Move::AllIn: roundContext.lastMove = Move::AllIn;
            roundContext.potSize += action.betAmount;
            roundContext.roundContributions[playerIdx] += action.betAmount;
            roundContext.prevRaise = action.raiseAmount; break;

        default:
            std::cout << "Invalid move sequence occurred here!:#\n";
    }
}

// we should spend the time to make this as scalable as possible for poker other than heads up
// let us treat amount as the amount to be raised
void Game::playStreet(Street street, RoundContext &roundContext) {
    // first, see if we have to draw some table cards
    switch (street) {
        case Street::Flop: table.flop(); break;
        case Street::Turn: table.turn(); break;
        case Street::River: table.river(); break;
        default:;
    }

    // see the board
    std::cout << table.boardToString() << "\n";

    const std::vector<BaseParticipant*> players = table.getPlayersInOrder(street);
    // if a raise occurs, this gets reset
    // if counter == num of players, we are done
    unsigned int counter = 0;
    // this get number of players is only for players NOT folded
    unsigned int numberOfPlayers = table.getNumberOfActivePlayers();


    while (counter != numberOfPlayers) {
        for (int i = 0; i < players.size(); i++) {

            if (players[i]->isFolded()) continue; // skip the rest of this if the player has folded

            if (dynamic_cast<Player*>(players[i])) {
                // only see the players hand, not the bots.
                std::cout << "Your hand is: " << players[i]->getHand() << "\n";
            }
            // we need to fix the fact we can still act when it is folded over to us
            const Action action = players[i]->act(roundContext, roundContext.amountToCall(i, players));


            updateRoundContext(roundContext, action, i);

            if (action.move != Move::Raise) {
                // when there is a raise, this changes the number of active players
                counter++;
            } else {
                counter = 0;
                numberOfPlayers = table.getNumberOfActivePlayers() - 1;
            }

            if (counter == numberOfPlayers) {
                break;
            }
        }
    }

    // now we report the end code via roundContext
    // so if the round is done, the game can be made aware
    numberOfPlayers = table.getNumberOfActivePlayers();
    if (numberOfPlayers > 1 && street == Street::River) roundContext.endCode = 2;
    else if (numberOfPlayers == 1) roundContext.endCode = 1;


}

// the end code might not even be useful
void Game::handleWinner(const RoundContext &roundContext) const {
    // everyone folded besides one person
    // so find that person, give them the chips
    const std::vector<std::unique_ptr<BaseParticipant>>& players = table.getPlayers();
    // where val is the idx
    std::vector<std::tuple<BaseParticipant*, u_int32_t>> handRankings;

    for (auto &p : players) {
        if (!p->isFolded()) {
            std::vector<Card> hand = p->getCards();
            std::vector<Card> fullHand = table.getFullHand(hand);
            std::tuple<BaseParticipant*, uint32_t> entry(p.get(), classifyHand(fullHand));
            handRankings.push_back(entry);
        }
    }

    std::sort(handRankings.begin(), handRankings.end(),
    [](const auto &a, const auto &b) {
        return std::get<1>(a) > std::get<1>(b);
    });

    // now we can take the first one, and give them the pot
    auto [winningPlayer, rank] = handRankings.front();
    winningPlayer->changeStack(roundContext.potSize, 1);

    std::cout << "Player won with the hand " << winningPlayer->getHand() << "\n";
}





