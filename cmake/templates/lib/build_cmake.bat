@REM This is example script for building with cmake

@REM Remove build directories beforehand
rd /s /q build_debug
rd /s /q build_release
rd /s /q output

@REM Note : CMAKE_RUNTIME_OUTPUT_DIRECTORY in CMakeLists.txt sets the runtime otuput directory
@REM Note : setting CMAKE_GENERATOR_TOOLSET doesn't work very well. just use cmake -T for setting toolset

cmake -A Win32 -B build_debug/x86 -DBUILDDEBUG=TRUE
cmake -A x64   -B build_debug/x64 -DBUILDDEBUG=TRUE
cmake -A Win32 -B build_release/x86
cmake -A x64   -B build_release/x64
cmake --build build_debug/x86   --config Debug
cmake --build build_debug/x64   --config Debug
cmake --build build_release/x86 --config Release
cmake --build build_release/x64 --config Release

