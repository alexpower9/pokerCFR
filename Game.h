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
#include "Table.h"
#include "Street.h"
// we should keep track of number of actions to ensure
// everyone acts during the street
// for now, lets just do heads up NLH
class Game {

public:
    Game(int bb, int sb, int stack)
    : bigBlind(bb),
      smallBlind(sb),
      table([&] {
          std::vector<std::unique_ptr<BaseParticipant>> v;
          v.push_back(std::make_unique<Player>(stack, Position::SB));
          v.push_back(std::make_unique<Player>(stack, Position::BB));
          v.push_back(std::make_unique<Player>(stack, Position::UTG));
          // v.push_back(std::make_unique<Bot>(stack, Position::BB));
          return Table(std::move(v));
      }())
    {}


    enum class HandRanking {HighCard, OnePair, TwoPair, Trips, Straight, Flush, FullHouse, Quads, StraightFlush};
    static const std::unordered_map<u_int32_t, std::string> rankToString;


    // deal the cards out to all the players
    // void startGame();

    // return an int for whichever player won the hand
    // just say that 0 = player, while 1 is the bot
    static u_int32_t classifyHand(const std::vector<Card> &hand);
    int evaluateHands(const std::vector<Card> &hand1, const std::vector<Card> &hand2);
    void runGame();

    // sort of redundant as all we are doing is pushing a card community cards
    // albeit 3 for the flop

    void seeBoard() const;
    static bool hasAce(const std::vector<Card> &hand);
    void reset_deck();
    void printGame(int pot) const;
    void printPositions() const;

    RoundContext createRoundContextForNextRound(Street street, unsigned int potSize, int endCode) const;
    RoundContext newHandContext(const std::vector<BaseParticipant*> &players) const;
    void updateRoundContext(RoundContext &roundContext, Action action, unsigned int playerIdx) const;
    void playStreet(Street street, RoundContext &roundContext);
    void handleWinner(const RoundContext &roundContext) const;
    inline bool onePlayerLeft() const;
    bool checkShowdown() const;


private:
    int bigBlind;
    int smallBlind;
    Deck deck = Deck::create();
    Table table;
};

#endif //CPPTUTORIAL_GAME_H