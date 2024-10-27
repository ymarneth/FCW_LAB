cmake_minimum_required(VERSION 3.29)
project(FCW1_LAB1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(FCW1_LAB1
    Grammar.cpp
    Grammar.h
    GrammarBasics.cpp
    GrammarBasics.h
    GrammarBuilder.cpp
    GrammarBuilder.h
    Main.cpp
    ObjectCounter.h
    SequenceStuff.cpp
    SequenceStuff.h
    SignalHandling.cpp
    SignalHandling.h
    SymbolStuff.cpp
    SymbolStuff.h
    Timer.cpp
    Timer.h
    Vocabulary.cpp
    Vocabulary.h)
