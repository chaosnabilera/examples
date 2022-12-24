# this is script only, so execute with cmake -P CMakeLists.txt

message("message prints things")

# note : all variables in CMake is treated as string!
set(msg0 "set sets variable.")
set(msg1 "note that")
set(MSG1 "variables are case sensitive, unlike preserved keywords")

# we can use variables like ${this}
# the following should print "set sets variable. note that variables are case sensitive, unlike preserved keywords"
message("${msg0} ${msg1} ${MSG1}")

# non-existing variables are treated as empty string
message("non-existent-variable : ${non-existent-variable}")

# unset unsets variable. now msg1 is non-existent after this
unset(msg1) 

message("msg1 after unset : ${msg1}")

# we can make a list variable like this
# note that when we print, each variables are separated by semicolon
set(c1 "c1")
set(c2 "c2")
set(c3 "c3")
set(concat ${c1} ${c2} ${c3}) 
message("concat : ${concat}")  # c1;c2;c3

# we don't have to use "" if we are dealing with single token string
set(c1 d1)
set(c2 d2)
set(c3 d3)
set(concat ${c1} ${c2} ${c3})
message("concat : ${concat}") # d1;d2;d3

# we can use variables for other variable's name + we can use variable inside variable
set(platform "win")

set(hello_win "hello windows")
set(hello_linux "hello linux")

message("hello_${platform} = ${hello_${platform}}") # hello_win = hello windows

# it is possible to set variables multi line
set(multiline_msg 
"hello
this
is
multi
line")

message(${multiline_msg})

# we can use [[]] instead of "". [[]] represents raw string, so we can't use variables inside it
set(ddmsg [[hello this is ddmsg]])
message(${ddmsg}) # hello this is ddmsg

# option lets you make boolean cache variables with default value 
# the following is equivalent too : set(option_bool1 ON CACHE BOOL "help message for option_bool1")
option(option_bool1 "help message for option_bool1" ON)
option(option_bool2 "help message for option_bool2" OFF)
set(option_bool3 ON CACHE BOOL "help message for option_bool3")
message("${option_bool1} ${option_bool2} ${option_bool3}") # ON OFF ON