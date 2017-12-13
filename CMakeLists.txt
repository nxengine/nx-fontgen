cmake_minimum_required(VERSION 2.8)
project(fontbm)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(SDL2  REQUIRED)
find_package(SDL2_ttf  REQUIRED)
find_package(PNG)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2TTF_INCLUDE_DIR})

add_definitions("-Wall -std=c++11")

add_subdirectory(src)
