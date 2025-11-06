cmake_minimum_required(VERSION 3.16)
set(CMAKE_CONFIGURE_DEPENDS ON)
project(CppTutorial)

set(CMAKE_CXX_STANDARD 20)

# actual poker game
file(GLOB MAIN_SRC src/pokergame/*.cpp)
add_executable(PokerEngine ${MAIN_SRC})
target_include_directories(PokerEngine PRIVATE src/pokergame)

# cfr training
file(GLOB_RECURSE CFR_SRC CONFIGURE_DEPENDS src/cfr/*.cpp)
add_executable(CFRTrainer ${CFR_SRC})
target_include_directories(CFRTrainer PRIVATE src src/cfr)