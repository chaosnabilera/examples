cmake_minimum_required(VERSION 3.0)
project(win_simple_socket_echo_client VERSION 1.0.0 LANGUAGES CXX)

add_executable(echo_client src/client.cpp)
target_link_libraries(echo_client ws2_32.lib)

add_executable(echo_server src/server.cpp)
target_link_libraries(echo_server ws2_32.lib)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} /MTd")
set(CMAKE_VERBOSE_MAKEFILE ON)