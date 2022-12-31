cmake_minimum_required(VERSION 3.0)
project(template_dll VERSION 1.0.0 LANGUAGES CXX)

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
set(SRC_DLL 
    src/main.cpp 
    src/module.cpp)

set(INCLUDE_DLL include)

set(LIB_DLL "") # if any *.lib is required

add_library(template_dll SHARED ${SRC_DLL})

# setting mode to PRIVATE would be more appropriate for dll in most cases
# consult cmake manual for detail
target_include_directories(template_dll PRIVATE ${INCLUDE_DLL})
target_compile_options(template_dll PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(template_dll PRIVATE ${LIB_DLL})

