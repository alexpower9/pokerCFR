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
#include "Bot.h"
#include "Player.h"
#include <tuple>

// for now, lets just do heads up NLH
class Game {


    Game(const int bigBlind, const int smallBlind, int startingStacks)
    : bigBlind(bigBlind), smallBlind(smallBlind), bot(startingStacks, Bot::Position::SB), player(startingStacks, Player::Position::BB)  {};


    enum class Street {PreFlop, Flop, Turn, River};
    enum class HandRanking {HighCard, OnePair, TwoPair, Trips, Straight, Flush, FullHouse, Quads, StraightFlush};


    // deal the cards out to all the players
    // void startGame();

    // return an int for whichever player won the hand
    // just say that 0 = player, while 1 is the bot
    std::tuple<HandRanking, int> classifyHand(std::vector<Card> hand);
    int evaluateHands(std::vector<Card> hand1, std::vector<Card> hand2);
    void runGame(Player &player, Bot &bot);

    // sort of redundant as all we are doing is pushing a card community cards
    // albeit 3 for the flop
    void flop();
    void turn();
    void river();

    void seeBoard() const;

    // maintain the current pot
    int pot_amount = 0;
    int iterations = 0;

private:
    std::vector<Card> communityCards;
    int bigBlind;
    int smallBlind;
    Bot bot;
    Player player;
    Deck deck = Deck::create();
};

#endif //CPPTUTORIAL_GAME_H