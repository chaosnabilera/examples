set(USE_DEBUG_CODE FALSE) # TRUE / FALSE

message("USE_DEBUG_CODE = ${USE_DEBUG_CODE}")

# note that using for condition, we don't need to to do ${USE_DEBUG_CODE}
if(USE_DEBUG_CODE)
    message("condition USE_DEBUG_CODE : true")
else()
    message("condition USE_DEBUG_CODE : false")
endif()

if(FALSE)
    message("condition FALSE : true")
else()
    message("condition FALSE : false")
endif()

if(NON_EXISTING_VARIABLE)
    message("condition NON_EXISTING_VARIABLE : true")
else()
    message("condition NON_EXISTING_VARIABLE : false")
endif()

# this creates following warning:
#   An argument named "TRUE" appears in a conditional statement.  Policy
#   CMP0012 is not set: if() recognizes numbers and boolean constants.  Run
#   "cmake --help-policy CMP0012" for policy details.  Use the cmake_policy
#   command to set the policy and suppress this warning.
#   This warning is for project developers.  Use -Wno-dev to suppress it.
if(TRUE)
    message("condition TRUE : true")
else()
    message("condition TRUE : false")
endif()

# there exist elseif as well
set(V1 FALSE)
set(V2 TRUE)
set(V3 FALSE)

message("")
if(V1)
    message("V1 checked as true first!")
elseif(V2)
    message("V2 checked as true first!")
else()
    message("V3 checked as true first!")
endif()