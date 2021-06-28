# - Try to find Iconv 
# Once done this will define 
# 
#  ICONV_FOUND - system has Iconv 
#  ICONV_INCLUDE_DIR - the Iconv include directory 
#  ICONV_LIBRARIES - Link these to use Iconv 
#  ICONV_SECOND_ARGUMENT_IS_CONST - the second argument for iconv() is const
#

include(CMakePushCheckState)
include(CheckCXXSourceCompiles)

find_path(ICONV_INCLUDE_DIR iconv.h)
 
find_library(ICONV_LIBRARIES NAMES iconv libiconv libiconv-2 c)

cmake_reset_check_state()
set(CMAKE_REQUIRED_FLAGS "-fno-permissive")
set(CMAKE_REQUIRED_INCLUDES ${ICONV_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${ICONV_LIBRARIES})
if(ICONV_INCLUDE_DIR AND ICONV_LIBRARIES)
    check_cxx_source_compiles("
    #include <iconv.h>
    int main(){
        iconv_t conv = 0;
        const char* in = 0;
        size_t ilen = 0;
        char* out = 0;
        size_t olen = 0;
        iconv(conv, &in, &ilen, &out, &olen);
        return 0;
    }
    " ICONV_SECOND_ARGUMENT_IS_CONST )
endif()
cmake_reset_check_state()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Iconv
    REQUIRED_VARS ICONV_INCLUDE_DIR ICONV_LIBRARIES
)

mark_as_advanced(
  ICONV_INCLUDE_DIR
  ICONV_LIBRARIES
  ICONV_SECOND_ARGUMENT_IS_CONST
)
