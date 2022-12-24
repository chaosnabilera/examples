# use cmake -P

foreach(cur a b c d e f)
    message("cur = ${cur}")
endforeach()

# note that variable substition does not occur like this
set(a 1)
set(b 2)
set(c 3)
message("")
foreach(cur a b c)
    message("cur = ${cur}")
endforeach()

# if we want to substitute in place, we need to like this
message("")
foreach(cur ${a} ${b} ${c})
    message("cur = ${cur}")
endforeach()

# or maybe like this
set(my_list m1 m2 m3)
message("")
foreach(cur ${my_list})
    message("cur = ${cur}")
endforeach()

# use ZIP_LIST if we want to combine lists
set(list_a a1 a2 a3)
set(list_b b1 b2 b3)
message("")
foreach(cur IN ZIP_LISTS list_a list_b)
    message("cur = ${cur_0} ${cur_1}")
endforeach()

# or do like this
message("")
foreach(e0 e1 IN ZIP_LISTS list_a list_b)
    message("cur = ${e0} ${e1}")
endforeach()

# we can do range based for
message("")
foreach(cur RANGE 0 9) # note that it is [0,9], not [0,9)
    message("cur = ${cur}")
endforeach()