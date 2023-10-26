cmake_minimum_required (VERSION 3.1)
project (SinkTheShip VERSION 1.0 LANGUAGES CXX)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3")
include_directories(".")

add_subdirectory(SinkTheShip)
include_directories("SinkTheShip")


add_executable(client client.hpp client.cpp)
target_link_libraries(client sinkTheShip)
target_compile_definitions(client PRIVATE -D__VERBOSE__=1)

add_executable(server server.hpp server.cpp)
target_link_libraries(server sinkTheShip)
