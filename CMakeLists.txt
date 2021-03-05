project (strigi)

# This is the aggregate build file. It serves only to call the build in the
# subdirectories. Each of the projects in the subdirectories is buildable
# separately.

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

# for testing to work in cmake, this command must be called in the root src dir
enable_testing()

if(POLICY CMP0017)
  cmake_policy(SET CMP0017 NEW)
endif()

# Used to report found packages
include(FeatureSummary)
# Used to create config files
include(CMakePackageConfigHelpers)
# Used to set some generic paths
include(GNUInstallDirs)
# Used to create strigi_export header
include(GenerateExportHeader)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# (Absolute) paths definitions
set(LIB_DESTINATION     "${CMAKE_INSTALL_FULL_LIBDIR}")
set(INCLUDE_DESTINATION "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
set(DATA_DESTINATION    "${CMAKE_INSTALL_FULL_DATAROOTDIR}")

# Set up RPATH handling, so the libs are found if they are installed to a non-standard location.
# By default cmake builds the targets with full RPATH to everything in the build directory,
# but then removes the RPATH when installing.
# These two options below make it set the RPATH of the installed targets to all
# RPATH directories outside the current CMAKE_BINARY_DIR and also the library
# install directory. Alex
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH  TRUE)
set(CMAKE_INSTALL_RPATH                ${LIB_DESTINATION} )

# due to a bug in libxml2, we only get thread support if _REENTRANT is defined
add_definitions(-D_REENTRANT)

# environment inspection
include(ConfigureChecks.cmake)

configure_file(
  ${CMAKE_SOURCE_DIR}/config.h.cmake
  ${CMAKE_BINARY_DIR}/config.h
)

include_directories(${CMAKE_BINARY_DIR})
add_definitions(-DHAVE_CONFIG_H)

# dependencies
find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES
    DESCRIPTION "Support for gzip compressed files and data streams"
    URL "http://www.zlib.net"
    TYPE REQUIRED
)

find_package(BZip2)
set_package_properties(BZip2 PROPERTIES
    DESCRIPTION "Support for BZip2 compressed files and data streams"
    URL "http://www.bzip.org"
    TYPE REQUIRED
)

find_package(Threads)
set_package_properties(Threads PROPERTIES
    DESCRIPTION "The thread library of the system"
    TYPE REQUIRED
)

find_package(File)
set_package_properties(File PROPERTIES
    URL "https://www.darwinsys.com/file/"
    TYPE REQUIRED
)

find_package(LibXml2)
set_package_properties(LibXml2 PROPERTIES
    URL "http://xmlsoft.org"
    TYPE REQUIRED
)

find_package(Iconv)
set_package_properties(Iconv PROPERTIES
    TYPE REQUIRED
)

find_package(Exiv2)
set_package_properties(Exiv2 PROPERTIES
    TYPE OPTIONAL
)

find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE)
set_package_properties(FFmpeg PROPERTIES
    TYPE OPTIONAL
)

find_package(XAttr)

# sub-projects
add_subdirectory(libstreams) # Must be first
add_subdirectory(libstreamanalyzer) # Must be second
add_subdirectory(strigiutils)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
