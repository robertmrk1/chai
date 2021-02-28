cmake_minimum_required(VERSION 3.2.0)
project(chai-training VERSION 0.1.0)


# Adding the-chess-library to the project
add_subdirectory(libs/the-chess-library build)
include_directories(libs/the-chess-library/include)

# Adding differential-evolution library to the project
include_directories(libs/differential-evolution/include)


add_executable(chai-training src/main.cpp)


# Linking (what ever that means) the-chess-library to the project
target_link_libraries(chai-training thc_chess_static)