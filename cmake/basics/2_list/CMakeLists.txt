# this is script only, so execute with cmake -P CMakeLists.txt

# so it turns out that following form is actually list assignment (the semi-colon thing)
set(MY_LIST a b c d eee fff g)
message("MY_LIST = ${MY_LIST}")

list(LENGTH MY_LIST MY_LIST_LENGTH)
message("MY_LIST_LENGTH = ${MY_LIST_LENGTH}")

# we can get each elment
list(GET MY_LIST 0 MY_LIST_0)
list(GET MY_LIST 1 MY_LIST_1)
list(GET MY_LIST 2 MY_LIST_2)

message("MY_LIST_0 = ${MY_LIST_0}")
message("MY_LIST_1 = ${MY_LIST_1}")
message("MY_LIST_2 = ${MY_LIST_2}")

# join connects all the elements of the list with specific string
list(JOIN MY_LIST "-" MY_LIST_JOINED)
message("MY_LIST_JOINED = ${MY_LIST_JOINED}")

# append appends to list
list(APPEND MY_LIST "hhHhh")
message("after APPEND : MY_LIST = ${MY_LIST}")

# we make list like the following
# note that ARB_LIST_LENGTH is 4!
set(ARB_LIST "a;b;c;d eee fff ggg")
list(LENGTH ARB_LIST ARB_LIST_LENGTH)
message("ARB_LIST = ${ARB_LIST}")
message("ARB_LIST_LENGTH = ${ARB_LIST_LENGTH}")
