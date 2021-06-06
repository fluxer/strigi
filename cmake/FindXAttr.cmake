# - Try to find XAttr header
#
# Once done this will define
#
#  XATTR_FOUND - system has XAttr
#  XATTR_INCLUDE_DIR - the XAttr include directory
#
# Copyright (c) 2021 Ivailo Monev <xakepa10@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(XATTR_INCLUDE_DIR
    NAMES attr/xattr.h
    HINTS $ENV{XATTRDIR}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XAttr
    REQUIRED_VARS XATTR_INCLUDE_DIR
)

mark_as_advanced(XATTR_INCLUDE_DIR)
