# test for some functions that are missing on a particular system
include(CheckFunctionExists)
include(CheckStructHasMember)

# libstreams/lib/mailinputstream.cpp
check_function_exists("strcasestr" HAVE_STRCASESTR)
# libstreams/lib/compat.cpp
check_function_exists("strlwr" HAVE_STRLWR)
# libstreams/lib/dostime.cpp
check_function_exists("localtime_r" HAVE_LOCALTIME_R)
# libstreamanalyzer/lib/analyzerloader.cpp, libstreamanalyzer/lib/fieldpropertiesdb.cpp
check_struct_has_member("struct dirent" "d_type" "dirent.h" HAVE_DIRENT_D_TYPE)

add_definitions(-DHAVE_CONFIG_H)
