//
// Created by Alex Power on 9/30/25.
//
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "Bot.h"
#include "RoundContext.h"
#include "Table.h"
#include "PotManager.h"
#include <vector>
#include "Move.h"
#include <cstdint>
#include <string>
#include <sstream>



std::string rankToString(int r) {
    // You chose numeric formatting:
    return std::to_string(r);
}

// Extractor: 5-bit chunks for up to 5 ranks
std::vector<int> extractRanks(uint32_t ranking) {
    static const int shifts[5] = {23, 18, 13, 8, 3};
    std::vector<int> out;
    out.reserve(5);
    for (int i = 0; i < 5; i++) {
        unsigned int r = (ranking >> shifts[i]) & 0x1F;
        if (r > 0) out.push_back(r);
    }
    return out;
}

std::string Game::decodeHandRank(uint32_t ranking) {
    unsigned int category = (ranking >> 28) & 0xF;
    std::vector<int> ranks = extractRanks(ranking);

    std::ostringstream out;

    switch (category) {
        case 8: { // Straight Flush
            out << "Straight Flush, " << rankToString(ranks[0]) << "-high";
            break;
        }
        case 7: { // Quads
            // ranks[0] = quad rank, ranks[1] = kicker
            out << "Four of a Kind, " << rankToString(ranks[0])
                << "s (kicker " << rankToString(ranks[1]) << ")";
            break;
        }
        case 6: { // Full House
            // ranks[0] = trips rank, ranks[1] = pair rank
            out << "Full House, " << rankToString(ranks[0])
                << "s full of " << rankToString(ranks[1]) << "s";
            break;
        }
        case 5: { // Flush (5 kickers stored)
            out << "Flush, ";
            for (size_t i = 0; i < ranks.size(); ++i) {
                out << rankToString(ranks[i]);
                if (i + 1 < ranks.size()) out << " ";
            }
            break;
        }
        case 4: { // Straight
            out << "Straight, " << rankToString(ranks[0]) << "-high";
            break;
        }
        case 3: { // Trips
            // ranks[0] = trips rank, then 2 kickers
            out << "Three of a Kind, " << rankToString(ranks[0]) << "s";
            if (ranks.size() > 1) {
                out << " (kickers ";
                for (size_t i = 1; i < ranks.size(); ++i) {
                    out << rankToString(ranks[i]);
                    if (i + 1 < ranks.size()) out << ", ";
                }
                out << ")";
            }
            break;
        }
        case 2: { // Two Pair
            // ranks[0] = high pair, ranks[1] = low pair, ranks[2] = kicker
            out << "Two Pair, " << rankToString(ranks[0]) << "s and "
                << rankToString(ranks[1]) << "s (kicker "
                << rankToString(ranks[2]) << ")";
            break;
        }
        case 1: { // One Pair
            // ranks[0] = pair, then 3 kickers
            out << "One Pair, " << rankToString(ranks[0]) << "s";
            if (ranks.size() > 1) {
                out << " (kickers ";
                for (size_t i = 1; i < ranks.size(); ++i) {
                    out << rankToString(ranks[i]);
                    if (i + 1 < ranks.size()) out << ", ";
                }
                out << ")";
            }
            break;
        }
        case 0: { // High Card
            // ranks[0] = highest, then next 4 kickers
            out << "High Card " << rankToString(ranks[0]);
            if (ranks.size() > 1) {
                out << " (";
                for (size_t i = 1; i < ranks.size(); ++i) {
                    out << rankToString(ranks[i]);
                    if (i + 1 < ranks.size()) out << ", ";
                }
                out << ")";
            }
            break;
        }
        default:
            out << "Unknown Hand";
    }

    return out.str();
}

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


    while (true) {
        std::cout << "\nNew hand is beginning!\n\n";

        std::vector<BaseParticipant*> initialOrder = table.getPlayersInOrder(Street::PreFlop);
        auto potManager = PotManager(initialOrder);
        RoundContext roundContext = newHandContext(bigBlind, smallBlind, initialOrder, potManager);
        table.beginRound(); // new deck, deal 2 cards

        for (int i = 0; i < ALL_STREETS.size(); i++) {
            const Street street = ALL_STREETS[i];
            std::cout << "Current street is " << streetToString(street) << "\n";
            const bool hasEnded = playStreet(street, roundContext, potManager);
            if (hasEnded) break;

            if (ALL_STREETS[i] != Street::River) {
                roundContext = createRoundContextForNextRound(ALL_STREETS[i + 1], roundContext.potSize, roundContext.endCode);
            }
        }

        // we are going to need the roundContext endCode (but not really)
        handleWinner(potManager);

        if (!table.movePositions()) {
            std::cout << "The game has ended!" << "\n";
            break;
        }
    }
}
// reminder that this will be given the full 7 Card hand, already sorted.
// THIS IS MY OLD VERSION, WHICH WAS TOO SLOW
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
                    ranking |= (flushCards[i - 3].getRank() & 0x1F) << 23;
                    // std::cout << "Rank of straight flush: " <<flushCards[i - 3].getRank() << "\n";
                    return ranking;
                }

                if (run == 4 && hasAce(hand) && flushCards[i + 1].getRank() == 2) {
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
    int tripsRank = 0;;
    for (int i = 0; i < 5; i++) {
        if (hand[i].getRank() == hand[i + 1].getRank() && hand[i + 1].getRank() == hand[i + 2].getRank()) {
            trips = true;
            tripsRank = hand[i].getRank();
            break;
           }
    }

    // check two pair and single pair at once
    int pairRank1 = 0;
    int pairRank2 = 0;
    for (int i = 0; i < hand.size() - 1; i++) {
        // pair found, check if we already found a pair though
        if (hand[i].getRank() == tripsRank) continue;

        if (hand[i].getRank() == hand[i + 1].getRank()) {
            if (pairRank1 == 0) {
                pairRank1 = hand[i].getRank();
            } else if (pairRank1 != 0 && hand[i].getRank() != pairRank1){
                // this must be the second pair
                pairRank2 = hand[i].getRank();
            }
        }
    }

    // now we return full house
    if (tripsRank != 0 && pairRank1 != 0) {
        ranking |= (6 & 0xF) << 28;
        ranking |= (tripsRank & 0x1F) << 23;
        ranking |= (pairRank1 & 0x1F) << 18; // no clue if this is actually a thing
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
                ranking |= (hand[i - 3].getRank() & 0x1F) << 23;
                return ranking;
            }

            if (run == 4 && hasAce(hand) && hand[i + 1].getRank() == 2) {
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

// check for an ace
bool Game::hasAce(const std::vector<Card> &hand) {
    if (hand[0].getRank() == 14) {
        return true;
    }

    return false;
}

RoundContext Game::newHandContext(const int bb, const int sb, const std::vector<BaseParticipant*> &players, PotManager &potManager) const {
    RoundContext newContext;
    newContext.bb = bigBlind;
    newContext.lastMove = Move::Call; // COULD BE BAD
    newContext.potSize = bigBlind + smallBlind;
    newContext.street = Street::PreFlop;
    newContext.prevRaise = 0;
    newContext.createRoundContributions(players);

    // note this will cause unsigned int overflow if the player has busted
    // for this use case we can leave it for now
    for (auto player : players) {
        if (player->getPosition() == Position::SB) {
            newContext.roundContributions[player] += sb;
            potManager.addContribution(player, sb);
            player->changeStack(sb, 0);
        } else if (player->getPosition() == Position::BB) {
            newContext.roundContributions[player] += bb;
            potManager.addContribution(player, bb);
            player->changeStack(bb, 0);
        }
    }

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
    newContext.createRoundContributions(table.getPlayersInOrder(street));
    return newContext;
}

//moves are in this order: Fold, Check, Bet, Call, Raise, All-in
//RoundContext has: callAmount, potSize, bb, prevRaise, street, lastMove
void Game::updateRoundContext(RoundContext &roundContext, Action action, BaseParticipant* player) const{
    switch (action.move) {
        case Move::Fold: roundContext.lastMove = Move::Fold; break;

        case Move::Call: roundContext.lastMove = Move::Call;
            roundContext.potSize += action.betAmount; roundContext.roundContributions[player] += action.betAmount;
            break;

        case Move::Check: roundContext.lastMove = Move::Check; break;

        case Move::Bet: roundContext.lastMove = Move::Bet;
            roundContext.prevRaise = action.raiseAmount;
            roundContext.potSize += action.betAmount; roundContext.roundContributions[player] += action.betAmount;
            break;

        case Move::Raise: roundContext.lastMove = Move::Raise;
            roundContext.potSize += action.betAmount;
            roundContext.roundContributions[player] += action.betAmount;
            roundContext.prevRaise = action.betAmount; break;

        case Move::AllIn: roundContext.lastMove = Move::AllIn;
            roundContext.potSize += action.betAmount;
            roundContext.roundContributions[player] += action.betAmount;
            roundContext.prevRaise = action.raiseAmount; break;

        default:
            std::cout << "Invalid move sequence occurred here!:#\n";
    }
}

// we should spend the time to make this as scalable as possible for poker other than heads up
// let us treat amount as the amount to be raised
// if this returns true, we can continue to the next street, if false, we end
bool Game::playStreet(Street street, RoundContext &roundContext, PotManager &potManager) {
    // Draw community cards
    switch (street) {
        case Street::Flop: table.flop(); break;
        case Street::Turn: table.turn(); break;
        case Street::River: table.river(); break;
        default:;
    }

    std::cout << table.boardToString() << "\n";

    std::vector<BaseParticipant*> players = table.getPlayersInOrder(street);

    // Track who has acted this round
    std::unordered_set<BaseParticipant*> hasActed;

    while (!onePlayerLeft()) {
        // Check if everyone is all-in except maybe one player
        if (checkShowdown()) {
            break;
        }

        // Check if betting round is complete
        if (isBettingComplete(players, hasActed, roundContext)) {
            break;
        }

        for (size_t i = 0; i < players.size() && !onePlayerLeft(); i++) {
            BaseParticipant* player = players[i];

            // Skip folded players or players with no chips
            if (player->isFolded() || player->getStack() == 0) {
                continue;
            }

            // Skip if this player has already acted in this betting round
            if (hasActed.count(player)) {
                continue;
            }

            // Show player's hand if human
            if (dynamic_cast<Player*>(player)) {
                std::cout << "Your hand is: " << player->getHand() << "\n";
            }

            // Get action
            const unsigned int amountToCall = roundContext.amountToCall(player);
            const Action action = player->act(roundContext, amountToCall);
            const unsigned int betAmount = action.betAmount;

            // Update context
            updateRoundContext(roundContext, action, player);
            potManager.addContribution(player, betAmount);
            hasActed.insert(player);

            // If this was a raise, everyone needs to act again
            if (betAmount > amountToCall) {
                hasActed.clear();
                hasActed.insert(player); // Raiser has acted
            }
        }
    }

    potManager.calculatePots(players);
    unsigned int numberOfPlayers = table.getNumberOfActivePlayers();

    // Hand is over if we're on river or only one player left
    return (numberOfPlayers > 1 && street == Street::River) || numberOfPlayers == 1;
}

bool Game::isBettingComplete(const std::vector<BaseParticipant*>& players, const std::unordered_set<BaseParticipant*>& hasActed,
                              const RoundContext& roundContext) const {
    // Everyone who can act must have acted
    for (auto* player : players) {
        if (player->isFolded() || player->getStack() == 0) {
            continue; // Skip folded/all-in players
        }

        if (!hasActed.count(player)) {
            return false; // This player hasn't acted yet
        }
    }

    // All active players must have matched the highest contribution
    unsigned int maxContribution = 0;
    for (const auto& [player, amount] : roundContext.roundContributions) {
        if (!player->isFolded()) {
            maxContribution = std::max(maxContribution, amount);
        }
    }

    for (auto* player : players) {
        if (player->isFolded()) continue;

        unsigned int contribution = roundContext.roundContributions.at(player);

        // Player must have either matched max contribution OR be all-in
        if (contribution < maxContribution && player->getStack() > 0) {
            return false;
        }
    }

    return true;
}

void Game::handleWinner(const PotManager &potManager) const {
    // std::cout << "Handling the winner\n";

    // so for each pot, we need to look over
    for (const auto &pot : potManager.getPots()) {
        // lets just copy here for now, this also accounts for folded players
        std::vector<BaseParticipant*> eligiblePlayers = pot.eligiblePlayers;
        std::vector<std::tuple<BaseParticipant*, u_int32_t>> handRankings;

        // now we go through the eligible players and rank their hand
        for (const auto &p: eligiblePlayers) {
            std::vector<Card> holeCards = p->getCards();
            std::vector<Card> fullHand = table.getFullHand(holeCards);
            std::tuple<BaseParticipant*, uint32_t> entry(p, classifyHand(fullHand));
            std::cout << p->getName() << " has " << decodeHandRank(std::get<1>(entry)) << "\n";
            handRankings.push_back(entry);
        }

        // now we have all the hand rankings for this pot
        std::sort(handRankings.begin(), handRankings.end(),[](const auto& a, const auto& b) {
              return std::get<1>(a) > std::get<1>(b);  // Higher value = better hand
          });

        uint32_t bestHandValue = std::get<1>(handRankings[0]);
        // now we take the players with the best hand, and divide the pot evenly
        std::vector<BaseParticipant*> winners;
        for (const auto& [player, handValue] : handRankings) {
            if (handValue == bestHandValue) {
                winners.push_back(player);
            } else {
                break;  // Since sorted, once we hit a lower value, we're done
            }
        }

        // now we have the winners for the pot
        awardPot(pot, winners);
    }
}

void Game::awardPot(const Pot& pot, const std::vector<BaseParticipant*>& winners) const {
    if (winners.empty()) {
        std::cerr << "Error: No winners for pot!" << std::endl;
        return;
    }

    unsigned int numWinners = winners.size();
    unsigned int amountPerWinner = pot.amount / numWinners;
    unsigned int remainder = pot.amount % numWinners;

    // Award chips to each winner
    for (int i = 0; i < winners.size(); i++) {
        unsigned int award = amountPerWinner;

        // First winner gets the odd chip(s)
        if (i == 0) {
            award += remainder;
        }

        winners[i]->changeStack(award, 1);

        // Display result
        if (numWinners > 1) {
            std::cout << winners[i]->getName() << " wins $" << award
                      << " (split pot with " << numWinners << " players)" << std::endl;
        } else {
            std::cout << winners[i]->getName() << " wins $" << award << std::endl;
        }
    }
}


inline bool Game::onePlayerLeft() const {
    return (table.getNumberOfActivePlayers() == 1); // getActivePlayers returns count of players not folded
}

bool Game::checkShowdown() const {
    // so if we have more than 1 active player, and they all have no chips, then
    // we can break the while loop and just progress the game
    unsigned int activePlayersCount = 0;
    unsigned int allInCount = 0;

    for (const auto &p:table.getPlayers()) {
        if (!p->isFolded()) activePlayersCount++;
        if (p->getStack() == 0) allInCount++;
    }

    return (activePlayersCount > 1 && activePlayersCount == allInCount);
};