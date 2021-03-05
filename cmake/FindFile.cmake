# - Try to find File
# Once done this will define
#
#  FILE_FOUND - system has File
#  FILE_INCLUDES - the File include directory
#  FILE_LIBRARIES - The libraries needed to use File
#
# Copyright (c) 2018 Ivailo Monev <xakepa10@gmail.com>
# Redistribution and use is allowed according to the terms of the BSD license.

# File does not provide pkg-config files

find_path(FILE_INCLUDES
    NAMES magic.h
    HINTS $ENV{FILEDIR}/include
)

find_library(FILE_LIBRARIES
    NAMES magic
    HINTS $ENV{FILEDIR}/lib
)

set(FILE_INCLUDE_DIR ${FILE_INCLUDES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(File
    REQUIRED_VARS FILE_INCLUDES FILE_LIBRARIES
)

mark_as_advanced(FILE_INCLUDES FILE_LIBRARIES)
