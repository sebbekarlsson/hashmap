cmake_minimum_required(VERSION 3.20)


project(hashmap_test)


file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_executable(hashmap_test ${sources})

target_compile_options(hashmap_test PUBLIC -fPIC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(hashmap_test PUBLIC hashmap)
