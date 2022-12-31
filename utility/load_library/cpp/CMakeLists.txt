cmake_minimum_required(VERSION 3.0)
project(loadlibrary VERSION 1.0.0 LANGUAGES CXX)

#
# Use following if this project is MSVC-only
#
# if(NOT MSVC)
#     message(FATAL_ERROR "This project is Visual C++ only")
# endif()

# Note : target_compile_options takes **list** variable as input
#        giving it string (something like A = "${A} blahblah") does not work!
set(PRJ_COMPILER_OPTION "")

# Add to list
list(APPEND PRJ_COMPILER_OPTION /W4)
list(APPEND PRJ_COMPILER_OPTION /permissive-)

# if-else
if(BUILDDEBUG)
    list(APPEND PRJ_COMPILER_OPTION /MTd)
else() # release mode
    list(APPEND PRJ_COMPILER_OPTION /MT)
endif()

# note that config name (Debug/Relase) is appended after CMAKE_GENERATOR_PLATFORM for visual studio
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output/${CMAKE_GENERATOR_PLATFORM}")

# print some info
message("CMAKE_GENERATOR_PLATFORM = ${CMAKE_GENERATOR_PLATFORM}")
message("CMAKE_GENERATOR_TOOLSET = ${CMAKE_GENERATOR_TOOLSET}")
message("BUILDDEBUG = ${BUILDDEBUG}")
message("PRJ_COMPILER_OPTION = ${PRJ_COMPILER_OPTION}")

# set compiler verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

# manage source files with variables
set(SRC_EXE 
    src/main.cpp)

set(INCLUDE_EXE include)

set(LIB_EXE "") # if any *.lib is required

add_executable(loadlibrary ${SRC_EXE})

# setting mode to PRIVATE would be more appropriate for exe in most cases
# consult cmake manual for detail
target_include_directories(loadlibrary PRIVATE ${INCLUDE_EXE})
target_compile_options(loadlibrary PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(loadlibrary PRIVATE ${LIB_EXE})

