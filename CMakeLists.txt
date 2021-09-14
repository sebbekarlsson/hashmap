cmake_minimum_required(VERSION 3.20)


project(hashmap)


file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(hashmap SHARED ${sources})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
