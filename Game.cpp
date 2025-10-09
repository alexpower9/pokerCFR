//
// Created by Alex Power on 9/30/25.
//
#include <iostream>
#include "Game.h"
#include "Player.h"

// each time it is the players turn, lets print this
void Game::printGame(const int pot) const {
    std::cout << "Current hand: ";
    player.seeHand();
    std::cout << "Stack: " << player.getStack() << " Pot: " << pot << "\n";
}

void Game::printPositions() const {
    std::cout << "Player is in " << Player::positionToString.at(player.getPosition()) << ", Bot is in " << Bot::positionToString.at(bot.getPosition()) << "\n";
}

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

const std::unordered_map<Game::Street, std::string> Game::streetToString = {
    {Game::Street::PreFlop, "Pre-flop"},
    {Game::Street::Flop, "Flop"},
    {Game::Street::Turn, "Turn"},
    {Game::Street::River, "River"},
};

void Game::reset_deck() {
    deck = Deck::create();
    deck.shuffle();
}

void Game::runGame() {
    while (true) {
        reset_deck(); // get a fresh deck each time, with it shuffled

        // deal cards to the players
        player.dealHand(deck);
        bot.dealHand(deck);

        std::cout << "New hand has begun!\n";
        std::cout << "Blinds are currently: " << bigBlind << "/" << smallBlind << "\n";
        int pot = 0;
        printPositions();
        // lets not worry about not having enough chips for now
        int botAmountPaid = bot.payBlind(bigBlind, smallBlind);
        int playerAmountPaid = player.payBlind(bigBlind, smallBlind);
        pot += bigBlind;
        pot += smallBlind;

        int playerAmountToPlay = bigBlind - playerAmountPaid;
        int botAmountToPlay = bigBlind - botAmountPaid;

        // pre flop action
        if (player.getPosition() == Player::Position::SB) {
            // then give user choice of what to do
            printGame(pot);
            Player::Move move = Player::move(playerAmountToPlay, bigBlind);

        } else {
            // for now lets just make the bot call the required amount
            // but will be useful in the future
            // this would just be the bot limping in
            bot.call(botAmountToPlay);
            pot += botAmountToPlay;
            std::cout << "Bot called for " << botAmountToPlay << "\n";

            // now player has to move
        }

        flop();
        printGame(pot);
        // need to determine positions again and such
        break;
    }
}

void Game::flop() {
    for (int i = 0; i < 3; i++) {
        communityCards.push_back(deck.draw());
    }

    std::cout << "Flop: ";
    for (const auto &c : communityCards) {
        std::cout << c.toString() << " ";
    }
    std::cout << "\n";
}

// maybe return 3 for a tie
int Game::evaluateHands(const std::vector<Card> &hand1, const std::vector<Card> &hand2) {
    std::vector<Card> combined1 = hand1;
    std::vector<Card> combined2 = hand2;

    combined1.insert(combined1.end(), communityCards.begin(), communityCards.end());
    combined2.insert(combined2.end(), communityCards.begin(), communityCards.end());

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

void Game::seeBoard() const {
    for (Card card: communityCards) {
        std::cout << card.toString() << " ";
    }
    std::cout << "\n";
}

// check for an ace
bool Game::hasAce(const std::vector<Card> &hand) {
    if (hand[0].getRank() == 14) {
        return true;
    }

    return false;
}

