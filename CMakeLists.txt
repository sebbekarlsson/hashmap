cmake_minimum_required(VERSION 3.20)


project(hashmap)


file(GLOB sources src/*.c)

add_library(hashmap SHARED ${sources})

include_directories(src/include)
