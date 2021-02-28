cmake_minimum_required(VERSION 3.2.0)

project(thc-chess)


include_directories(include) 


add_library(thc_chess SHARED src/thc.cpp)
add_library(thc_chess_static STATIC src/thc.cpp) 