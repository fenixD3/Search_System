cmake_minimum_required(VERSION 3.16)
project(Final_search_system_server_with_synhronize)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -pthread)

include_directories(.)

add_executable(Final_search_system_server_with_synhronize
        iterator_range.h
        main.cpp
        new_profile.cpp
        new_profile.h
        parse.cpp
        parse.h
        profile.h
        search_server.cpp
        search_server.h
        synhronizes.h
        test_runner.h splitwords.cpp)
