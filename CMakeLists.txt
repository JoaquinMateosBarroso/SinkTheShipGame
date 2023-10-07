cmake_minimum_required (VERSION 3.1)
project (SinkTheShip VERSION 1.0 LANGUAGES CXX)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
include_directories(".")

add_subdirectory(User)
add_subdirectory(SinkTheShip)
include_directories("User" "SinkTheShip")


add_executable(client sinkTheShipClient.hpp sinkTheShipClient.cpp)
target_compile_definitions(client PRIVATE -D__VERBOSE__=1)

add_executable(server sinkTheShipServer.hpp sinkTheShipServer.cpp)
target_compile_definitions(server PRIVATE -D__VERBOSE__=1)
