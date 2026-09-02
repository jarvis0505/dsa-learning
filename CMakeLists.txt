cmake_minimum_required(VERSION 4.0)
project(Final_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Final_Project
        BookmarkManager.cpp
        BookmarkManager.h
        BracketChecker.cpp
        BracketChecker.h
        BracketInfo.h
        Document.cpp
        Document.h
        EditRecord.h
        HashTable.h
        LinkedList.h
        main.cpp
        Stack.h
        WordIndex.cpp
        WordIndex.h)
