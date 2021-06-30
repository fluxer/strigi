project(libstreamanalyzer)

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
    ${FILE_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

add_subdirectory(lib)
add_subdirectory(plugins)

# all installed header files are listed here
file(GLOB STRIGI_HEADERS include/strigi/*.h)
install(
    FILES ${STRIGI_HEADERS}
    DESTINATION ${INCLUDE_DESTINATION}/strigi
)

# ontologies
file(GLOB STRIGI_ONTOLOGIES share/strigi/fieldproperties/*.rdfs)
install(
    FILES ${STRIGI_ONTOLOGIES}
    DESTINATION ${DATA_DESTINATION}/strigi/fieldproperties/
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libstreamanalyzer.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/lib/libstreamanalyzer.pc
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/lib/libstreamanalyzer.pc
    DESTINATION ${LIB_DESTINATION}/pkgconfig
)
