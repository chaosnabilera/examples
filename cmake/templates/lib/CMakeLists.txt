cmake_minimum_required(VERSION 3.0)
project(template_lib VERSION 1.0.0 LANGUAGES CXX)

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
set(SRC_LIB 
    src/libfunc1.cpp 
    src/libfunc2.cpp)

set(INCLUDE_LIB include)

set(LIB_LIB "") # if any *.lib is required

add_library(template_lib STATIC ${SRC_LIB})

# lib needs this in addition to set actual output place
set_target_properties(template_lib PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# consult cmake manual for detail

# this should be PUBLIC in most cases
target_include_directories(template_lib PUBLIC ${INCLUDE_LIB})

# this should be PRIVATE in most cases
target_compile_options(template_lib PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(template_lib PRIVATE ${LIB_LIB})

