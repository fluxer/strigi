# - Try to find Regex header
#
# Once done this will define
#
#  REGEX_FOUND - system has Regex
#  REGEX_INCLUDE_DIR - the Regex include directory
#
# Copyright (c) 2021 Ivailo Monev <xakepa10@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(REGEX_INCLUDE_DIR
    NAMES regex.h
    HINTS $ENV{REGEXDIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Regex
    REQUIRED_VARS REGEX_INCLUDE_DIR
)

mark_as_advanced(REGEX_INCLUDE_DIR)
