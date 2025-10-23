cmake_minimum_required(VERSION 4.0)
project(CppTutorial)

set(CMAKE_CXX_STANDARD 20)

add_executable(CppTutorial main.cpp
        Card.h
        Card.cpp
        Deck.h
        Deck.cpp
        Player.h
        Game.h
        Player.cpp
        Bot.h
        Bot.cpp
        Game.cpp
        BaseParticipant.h
        Move.h
        Position.h
        Action.h
        BaseParticipant.cpp
        Table.h
        Table.cpp
        RoundContext.h
        Street.h
)

add_executable(CppTutorial sandbox.cpp)
