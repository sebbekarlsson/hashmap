cmake_minimum_required(VERSION 3.20)


project(hashmap)


file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(hashmap SHARED ${sources})
add_library(hashmap_static STATIC ${sources})

add_executable(hashmap_e ${sources})

target_compile_options(hashmap PUBLIC -fPIC)
target_compile_options(hashmap_static PUBLIC -fPIC)
#target_compile_options(hashmap_e PUBLIC -fPIC -g -Wall)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)


target_include_directories(
  hashmap_e
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(
  hashmap
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_include_directories(
  hashmap_static
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      ${CMAKE_CURRENT_BINARY_DIR}/include
      ${CMAKE_CURRENT_SOURCE_DIR}/include)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
