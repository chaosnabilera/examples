@REM This is example script for building with cmake

@REM Remove build directories beforehand
rd /s /q build_debug
rd /s /q build_release

@REM if not exist output
@REM     mkdir output
@REM if not exist output\debug
@REM     mkdir output\debug
@REM if not exist output\release
@REM     mkdir output\release

@REM Note that setting CMAKE_GENERATOR_TOOLSET doesn't work very well. just use cmake -T for setting toolset

@REM cmake -A Win32 -B debug/x86 -T v141_xp -DBUILDDEBUG=TRUE
@REM cmake -A x64   -B debug/x64 -T v141_xp -DBUILDDEBUG=TRUE
@REM cmake -A Win32 -B release/x86 -T v141_xp
@REM cmake -A x64   -B release/x64 -T v141_xp
@REM cmake --build debug/x86   --config Debug
@REM cmake --build debug/x64   --config Debug
@REM cmake --build release/x86 --config Release
@REM cmake --build release/x64 --config Release

cmake -A Win32 -B build_debug/x86 -DBUILDDEBUG=TRUE
cmake -A x64   -B build_debug/x64 -DBUILDDEBUG=TRUE
cmake -A Win32 -B build_release/x86
cmake -A x64   -B build_release/x64
cmake --build build_debug/x86   --config Debug
cmake --build build_debug/x64   --config Debug
cmake --build build_release/x86 --config Release
cmake --build build_release/x64 --config Release