# headers that may or may not be present on the system
INCLUDE(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX(regex.h HAVE_REGEX_H)            # strigiutils/*

# test for some functions that are missing on a particular system
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(isblank HAVE_ISBLANK)             # libstreams/lib/mailinputstream.cpp
CHECK_FUNCTION_EXISTS(mkstemp HAVE_MKSTEMP)             # libstreamanalyzer/lib/endanalyzers/helperendanalyzer.cpp
CHECK_FUNCTION_EXISTS(setenv HAVE_SETENV)               # strigiutils/bin/xmlindexer/peranalyzerxml.cpp
CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)       # libstreamanalyzer/lib/fieldpropertiesdb.cpp
CHECK_FUNCTION_EXISTS(strcasestr HAVE_STRCASESTR)       # libstreams/lib/mailinputstream.cpp
CHECK_FUNCTION_EXISTS(strlwr HAVE_STRLWR)               # libstreams/lib/compat.cpp
CHECK_FUNCTION_EXISTS(strncasecmp HAVE_STRNCASECMP)     # libstreams/lib/mailinputstream.cpp
CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)     # libstreams/lib/dostime.cpp

ADD_DEFINITIONS(-DHAVE_CONFIG_H)
