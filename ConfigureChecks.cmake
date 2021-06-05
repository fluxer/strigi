# headers that may or may not be present on the system
INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(regex.h HAVE_REGEX_H)            # src/dummyindexer/*

# test for some functions that are missing on a particular system
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(isblank HAVE_ISBLANK)             # src/streams/mailinputstream.cpp, src/streams/strigi/compat.cpp
CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)             # src/streamanalyzer/helperendanalyzer.cpp
CHECK_FUNCTION_EXISTS(setenv HAVE_SETENV)               # src/xmlindexer/peranalyzerxml.cpp
CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)       # src/streamindexer/expatsaxendanalyzer.cpp, src/streamindexer/saxendanalyzer.cpp
CHECK_FUNCTION_EXISTS(strcasestr HAVE_STRCASESTR)       # src/streams/mailinputstream.cpp
CHECK_FUNCTION_EXISTS(strlwr HAVE_STRLWR)               # libstreams/lib/compat.cpp
CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)     # src/streams/mailinputstream.cpp
CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)     # src/streams/dostime.cpp

# now write out our configuration....
ADD_DEFINITIONS(-DHAVE_CONFIG_H)
