# test for some functions that are missing on a particular system
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(strcasestr HAVE_STRCASESTR)       # libstreams/lib/mailinputstream.cpp
CHECK_FUNCTION_EXISTS(strlwr HAVE_STRLWR)               # libstreams/lib/compat.cpp
CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)     # libstreams/lib/dostime.cpp

ADD_DEFINITIONS(-DHAVE_CONFIG_H)
