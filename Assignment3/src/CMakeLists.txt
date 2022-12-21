cmake_minimum_required(VERSION 3.16)
project(fcm)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -pthread")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../bin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable (fcm fcm.cpp)
target_link_libraries (fcm)

add_executable (lang lang.cpp lang.h)
target_link_libraries (lang)

add_executable (findlang findlang.cpp lang.h )
target_link_libraries (findlang)

add_executable (locatelang locatelang.cpp lang.h)
target_link_libraries (locatelang)