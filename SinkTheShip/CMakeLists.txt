cmake_minimum_required (VERSION 3.1)
project (SinkTheShip VERSION 1.0 LANGUAGES CXX)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
include_directories(".")



add_library(sinkTheShip sinkTheShip.cpp GameManager.cpp Handlers.cpp)
target_include_directories(sinkTheShip PUBLIC ${CMAKE_SOURCE_DIR}/SinkTheShip)