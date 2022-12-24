# this is script only, so execute with cmake -P CMakeLists.txt
# reference : https://cmake.org/cmake/help/latest/command/string.html

message("")

# find substring inside string
set(haystack "do not go gentle into that good night gentle sir")
set(needle "gentle")
string(FIND ${haystack} ${needle} find_result)
string(FIND ${haystack} ${needle} reverse_find_result REVERSE)
message("find \"${needle}\" inside \"${haystack}\" : ${find_result}") # 10 (index is zero-based)
message("find reverse \"${needle}\" inside \"${haystack}\" : ${reverse_find_result}") # 38 (index is zero-based)

message("")

# replace string
set(template "xxx is a very popular os. go xxx!")
string(REPLACE "xxx" "windows" twin ${template})
string(REPLACE "xxx" "linux" tlinux ${template})
message("template = ${template}")
message("    replace xxx with windows = ${twin}")
message("    replace xxx with linux = ${tlinux}")

message("")

# append string
set(append_input "append start : ")
string(APPEND append_input "input1! ")
string(APPEND append_input "input2! ")
message("append_input = ${append_input}")