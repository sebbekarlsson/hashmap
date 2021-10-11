cmake_minimum_required(VERSION 3.20)


project(hashmap)


file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(hashmap SHARED ${sources})

target_compile_options(hashmap PRIVATE -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)


target_include_directories(
  hashmap
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)
