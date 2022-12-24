cmake_minimum_required(VERSION 3.0)
project(win_tcp_echo VERSION 1.0.0 LANGUAGES CXX)

if(NOT MSVC)
    message(FATAL_ERROR "This project is Visual C++ only")
endif()

# Note : target_compile_options takes **list** variable as input
#        giving it string (something like A = "${A} blahblah") does not work!
set(PRJ_COMPILER_OPTION "")

if(CMAKE_GENERATOR_TOOLSET STREQUAL v141_xp)
    list(APPEND PRJ_COMPILER_OPTION /W4)
else()
    # list(APPEND PRJ_COMPILER_OPTION /Wall)
    # list(APPEND PRJ_COMPILER_OPTION /WX) # urg... warning in stl...
    list(APPEND PRJ_COMPILER_OPTION /W4)
    list(APPEND PRJ_COMPILER_OPTION /permissive-)
endif()

if(BUILDDEBUG)
    list(APPEND PRJ_COMPILER_OPTION /MTd)
else() # release mode
    list(APPEND PRJ_COMPILER_OPTION /MT)
endif()

message("CMAKE_GENERATOR_TOOLSET = ${CMAKE_GENERATOR_TOOLSET}")
message("BUILDDEBUG = ${BUILDDEBUG}")
message("PRJ_COMPILER_OPTION = ${PRJ_COMPILER_OPTION}")

add_executable(echo_client src/client.cpp src/tcp_common.cpp)
target_include_directories(echo_client PRIVATE include)
target_compile_options(echo_client PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(echo_client PRIVATE ws2_32.lib)

add_executable(echo_server src/server.cpp src/tcp_common.cpp)
target_include_directories(echo_server PRIVATE include)
target_compile_options(echo_server PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(echo_server PRIVATE ws2_32.lib)

set(CMAKE_VERBOSE_MAKEFILE ON)