project (libstreamanalyzer)

##### building and testing #####

configure_file(
    ${libstreamanalyzer_SOURCE_DIR}/strigi_thread.h.cmake
    ${libstreamanalyzer_BINARY_DIR}/include/strigi/strigi_thread.h
)

# every directory needs the headers that will be installed
include_directories(
    ${libstreams_SOURCE_DIR}/include
    ${libstreams_BINARY_DIR}/include
    ${libstreamanalyzer_SOURCE_DIR}/include
    ${libstreamanalyzer_SOURCE_DIR}/include/strigi
    ${libstreamanalyzer_BINARY_DIR}/include
    ${libstreamanalyzer_BINARY_DIR}/include/strigi
    ${libstreamanalyzer_SOURCE_DIR}/lib
    ${libstreamanalyzer_BINARY_DIR}/lib
    ${ICONV_INCLUDE_DIR}
    ${EXIV2_INCLUDE_DIR}
    ${FFMPEG_INCLUDE_DIRS}
    ${FILE_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

add_subdirectory(lib)
add_subdirectory(plugins)
#add_subdirectory(tests)

##### installing #####

# all installed files are listed here
# headers
file(GLOB STRIGI_HEADERS include/strigi/*.h)
install(
    FILES
    ${STRIGI_HEADERS}
    ${libstreamanalyzer_BINARY_DIR}/include/strigi/strigi_thread.h
    DESTINATION ${INCLUDE_DESTINATION}/strigi
)

# ontologies
file(GLOB STRIGI_ONTOLOGIES share/strigi/fieldproperties/*.rdfs)
install(
    FILES ${STRIGI_ONTOLOGIES}
    DESTINATION ${DATA_DESTINATION}/strigi/fieldproperties/
)

# library
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libstreamanalyzer.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/lib/libstreamanalyzer.pc
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libstreamanalyzer.pc
    DESTINATION ${LIB_DESTINATION}/pkgconfig
)
