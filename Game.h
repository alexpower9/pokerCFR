//
// Created by Alex Power on 9/25/25.
//

#ifndef CPPTUTORIAL_GAME_H
#define CPPTUTORIAL_GAME_H
#include <vector>
#include "Bot.h"
#include "Player.h"
#include "Deck.h"
#include "Card.h"

// for now, lets just do heads up NLH
class Game {

public:
    Game(const int bigBlind, const int smallBlind, const int startingStacks)
    : bigBlind(bigBlind), smallBlind(smallBlind), bot(startingStacks, Bot::Position::BB), player(startingStacks, Player::Position::SB)  {};

    enum class Street {PreFlop, Flop, Turn, River};
    enum class HandRanking {HighCard, OnePair, TwoPair, Trips, Straight, Flush, FullHouse, Quads, StraightFlush};
    static const std::unordered_map<u_int32_t, std::string> rankToString;
    static const std::unordered_map<Street, std::string> streetToString;


    // deal the cards out to all the players
    // void startGame();

    // return an int for whichever player won the hand
    // just say that 0 = player, while 1 is the bot
    static u_int32_t classifyHand(const std::vector<Card> &hand);
    int evaluateHands(const std::vector<Card> &hand1, const std::vector<Card> &hand2);
    void runGame();

    // sort of redundant as all we are doing is pushing a card community cards
    // albeit 3 for the flop
    void flop();
    void turn();
    void river();

    void seeBoard() const;
    static bool hasAce(const std::vector<Card> &hand);
    void reset_deck();
    void printGame(int pot) const;
    void printPositions() const;

    Street currentStreet = Street::PreFlop;


private:
    std::vector<Card> communityCards;
    int bigBlind;
    int smallBlind;
    Bot bot;
    Player player;
    Deck deck = Deck::create();
};

#endif //CPPTUTORIAL_GAME_H