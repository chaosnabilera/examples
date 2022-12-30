cmake_minimum_required(VERSION 3.0.0)
project(winwrap VERSION 1.0.0 LANGUAGES CXX)

if(NOT MSVC)
	message(FATAL_ERROR "This project is for Visual C++ only")
endif()

# Note : target_compile_options takes **list** variable as input
#        giving it string (something like A = "${A} blahblah") does not work!
set(PRJ_COMPILER_OPTION /W4 /permissive-)

if(BUILDDEBUG)
    list(APPEND PRJ_COMPILER_OPTION /MTd)
else() # release mode
    list(APPEND PRJ_COMPILER_OPTION /MT)
endif()

# set output directory
# note that config name (Debug/Relase) is appended after CMAKE_GENERATOR_PLATFORM for visual studio
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/output/${CMAKE_GENERATOR_PLATFORM}")

# print some info
message("CMAKE_GENERATOR_PLATFORM = ${CMAKE_GENERATOR_PLATFORM}")
message("CMAKE_GENERATOR_TOOLSET = ${CMAKE_GENERATOR_TOOLSET}")
message("PRJ_COMPILER_OPTION = ${PRJ_COMPILER_OPTION}")
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY = ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("CMAKE_LIBRARY_OUTPUT_DIRECTORY = ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")

# set global include directory
include_directories(src/include)

# set compiler verbose mode
set(CMAKE_VERBOSE_MAKEFILE ON)

# set sources for each project
# Note : compression library works for windows 8 or later
set(SRC_COMPRESSION src/compression/winwrap_compression.cpp) 

set(SRC_ENCODING src/encoding/winwrap_encoding.cpp)

set(SRC_FILESYSTEM 
	src/filesystem/winfile.cpp 
	src/filesystem/winfilesystemwalk.cpp 
	src/filesystem/winifileoperation.cpp 
	src/filesystem/winpath.cpp)

set(SRC_REGISTRY 
	src/registry/winwrap_registry_a.cpp 
	src/registry/winwrap_registry_w.cpp)

set(SRC_TCP 
	src/tcp/sock.cpp 
	src/tcp/winacceptsock.cpp 
	src/tcp/winsock.cpp 
	src/tcp/wintcp.cpp)

set(SRC_WINCRYPT 
	src/wincrypt/wincrypt_aes256.cpp 
	src/wincrypt/wincrypt_common.cpp 
	src/wincrypt/wincrypt_sha256.cpp)

set(SRC_TEST 
	src/test/main.cpp 
	src/test/test_compression.cpp 
	src/test/test_echoclient.cpp 
	src/test/test_echoserver.cpp 
	src/test/test_filesystem.cpp 
	src/test/test_registry.cpp 
	src/test/test_wincrypt.cpp)

# define projects

# Note : compression library works for windows 8 or later
add_library(winwrap_compression STATIC ${SRC_COMPRESSION})
set_target_properties(winwrap_compression PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_compression PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(winwrap_compression PUBLIC Cabinet.lib)

add_library(winwrap_encoding STATIC ${SRC_ENCODING})
set_target_properties(winwrap_encoding PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_encoding PRIVATE ${PRJ_COMPILER_OPTION})

add_library(winwrap_filesystem STATIC ${SRC_FILESYSTEM})
set_target_properties(winwrap_filesystem PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_filesystem PRIVATE ${PRJ_COMPILER_OPTION})

add_library(winwrap_registry STATIC ${SRC_REGISTRY})
set_target_properties(winwrap_registry PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_registry PRIVATE ${PRJ_COMPILER_OPTION})

add_library(winwrap_tcp STATIC ${SRC_TCP})
set_target_properties(winwrap_tcp PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_tcp PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(winwrap_tcp PUBLIC ws2_32.lib)

add_library(winwrap_wincrypt STATIC ${SRC_WINCRYPT})
set_target_properties(winwrap_wincrypt PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_wincrypt PRIVATE ${PRJ_COMPILER_OPTION})

add_executable(winwrap_test ${SRC_TEST})
set_target_properties(winwrap_test PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
target_compile_options(winwrap_test PRIVATE ${PRJ_COMPILER_OPTION})
target_link_libraries(winwrap_test
	winwrap_compression
	winwrap_encoding
	winwrap_filesystem
	winwrap_registry
	winwrap_tcp
	winwrap_wincrypt
)