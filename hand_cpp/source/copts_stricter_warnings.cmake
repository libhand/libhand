CMAKE_MINIMUM_REQUIRED (VERSION 2.6.3)

# Warnings
# We enable the following warnings and treat them as errors
#
# The user is *strongly encouraged* not to change these
# If there is a subproject that is very difficult to compile
# because of a warning mentioned here,
# REMOVE_DEFINITIONS(-Werror=something) 
# can be used to disable a warning for a subdirectory
# or even ADD_DEFINITIONS(-Wno-error)
# to ignore all warnings
ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-Werror=address)
ADD_DEFINITIONS(-Werror=char-subscripts)
ADD_DEFINITIONS(-Werror=comment)
ADD_DEFINITIONS(-Werror=format)
ADD_DEFINITIONS(-Werror=missing-include-dirs)
ADD_DEFINITIONS(-Werror=parentheses)
ADD_DEFINITIONS(-Werror=reorder)
ADD_DEFINITIONS(-Werror=return-type)
ADD_DEFINITIONS(-Werror=sequence-point)
#ADD_DEFINITIONS(-Werror=sign-compare)
ADD_DEFINITIONS(-Werror=strict-aliasing)
ADD_DEFINITIONS(-Werror=switch)
ADD_DEFINITIONS(-Werror=trigraphs)
ADD_DEFINITIONS(-Werror=uninitialized)
ADD_DEFINITIONS(-Werror=unknown-pragmas)
ADD_DEFINITIONS(-Werror=unused-function)
ADD_DEFINITIONS(-Werror=unused-variable)
ADD_DEFINITIONS(-Werror=volatile-register-var)
