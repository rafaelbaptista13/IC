cmake_minimum_required(VERSION 3.16)
project(opencv)

find_package(OpenCV REQUIRED)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17 -pthread")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../opencv-bin)

add_executable (cp_image cp_image.cpp)
target_link_libraries (cp_image ${OpenCV_LIBS})

add_executable (audio_encoder audio_encoder.cpp)
target_link_libraries (audio_encoder sndfile)

add_executable (audio_decoder audio_decoder.cpp)
target_link_libraries (audio_decoder sndfile)

add_executable (image_encoder image_encoder.cpp)
target_link_libraries (image_encoder ${OpenCV_LIBS})

add_executable (image_decoder image_decoder.cpp)
target_link_libraries (image_decoder ${OpenCV_LIBS})