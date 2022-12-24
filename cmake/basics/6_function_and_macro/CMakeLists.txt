# cmake -P CMakeLists.txt

# function can be defined in CMake
message("<1>")
function(f1 A B)
    message("f1 A = ${A}")
    message("f1 B = ${B}")
endfunction()

f1(a1 a2)

# functions have predefined keywords
message("<2>")
function(f mandatory1 mandatory2)
    message("mandatory1 = ${mandatory1}    mandatory2 = ${mandatory2}")
    message("ARGC=${ARGC}")
    message("ARGV=${ARGV}")
    message("ARGN=${ARGN}")
    foreach(optional_arg IN LISTS ARGN)
        message("    optional_arg=${optional_arg}")
    endforeach()
endfunction()

f(m1 m2)
message("")
f(m1 m2 optional1 optional2 optional3)

# function can be nested
function(f2 C)
    f1(a1 a2)
    message("f2 C = ${C}")
endfunction()

message("<3>")
f2(b1)

# function have its own scope
function(f1)
    set(A 1)
    set(B 2)
endfunction()

function(f2)
    set(A 3)
    set(B 4)
    f1(a1 a2)
    message("A = ${A}") # note that calling f1 did not change values of A,B
    message("B = ${B}")
endfunction()

message("<4>")
f2()

# however, macro is different from function
# macro is basically works like string replacement
macro(m1)
    set(A 1)
    set(B 2)
endmacro()

function(f2)
    set(A 3)
    set(B 4)
    m1(a1 a2)
    message("A = ${A}") # note that calling m1 did change values of A,B
    message("B = ${B}")
endfunction()

message("<5>")
f2()


# macro is just string replacement, so it doesn't create its own scope. different from function
macro(m1)
    message("A = ${A}")
    message("B = ${B}")
endmacro()

function(f1)
    m1() # A and B are not defined here
    set(A "AA")
    set(B "BB")
    m1() # A and B are defined here
endfunction()

message("<6>")
f1()

# we can call function from other cmakefile by using include
include("include_example.cmake")
message("<7>")
include_func()