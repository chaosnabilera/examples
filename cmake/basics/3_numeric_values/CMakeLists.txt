set(x 1)
set(y 3)
set(z 4)

# (x+y)*z

math(EXPR result "(${x}+${y})*${z}")
message("(${x}+${y})*${z} = ${result}")

# Note that CMake treats all variables as string

set(x "5")
set(y "6")
set(z "7")

# (x+y)*z

math(EXPR result "(${x}+${y})*${z}")
message("(${x}+${y})*${z} = ${result}")

# Non-numeric values are ignored (warning appears though)

set(x "1 haha")
set(y "2 hoohoo")
set(z "3 hehe")

# (x+y)*z

math(EXPR result "(${x}+${y})*${z}")
message("(${x}+${y})*${z} = ${result}")