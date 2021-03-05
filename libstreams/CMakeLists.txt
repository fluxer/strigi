project (libstreams)

# libstreams defines the definitions shared by the Strigi projects
set(STRIGI_VERSION_MAJOR 0 CACHE INT "Major Strigi version number" FORCE)
set(STRIGI_VERSION_MINOR 8 CACHE INT "Minor Strigi version number" FORCE)
set(STRIGI_VERSION_PATCH 0 CACHE INT "Release Strigi version number" FORCE)
set(STRIGI_VERSION_STRING "${STRIGI_VERSION_MAJOR}.${STRIGI_VERSION_MINOR}.${STRIGI_VERSION_PATCH}" CACHE STRING "Strigi version string" FORCE)
set(LIBSTREAMS_VERSION ${STRIGI_VERSION_STRING})

##### environment inspection #####

# Generate include/strigi/strigiconfig.h
configure_file(
    ${libstreams_SOURCE_DIR}/strigiconfig.h.cmake
    ${libstreams_BINARY_DIR}/include/strigi/strigiconfig.h
)

##### building and testing #####
# every directory needs the headers that will be installed
include_directories(
    ${libstreams_SOURCE_DIR}/include
    ${libstreams_BINARY_DIR}/include
    ${libstreams_SOURCE_DIR}/lib
    ${libstreams_BINARY_DIR}/lib
    ${ZLIB_INCLUDE_DIR}
    ${BZIP2_INCLUDE_DIR}
    ${ICONV_INCLUDE_DIR}
)

add_subdirectory(lib)
add_subdirectory(tests)

# Create strigi_export header
generate_export_header(streams BASE_NAME strigi)
configure_file(
    ${CMAKE_CURRENT_BINARY_DIR}/strigi_export.h
    include/strigi/strigi_export.h
)

##### installing #####

# all installed files are listed here
# headers
file(GLOB STRIGI_HEADERS include/strigi/*.h)
install(
    FILES
    ${STRIGI_HEADERS}
    ${libstreams_BINARY_DIR}/include/strigi/strigiconfig.h
    ${CMAKE_CURRENT_BINARY_DIR}/strigi_export.h
    DESTINATION ${INCLUDE_DESTINATION}/strigi
)
