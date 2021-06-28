set(streams_SRCS
    archivereader.cpp
    archiveentrycache.cpp
    listinginprogress.cpp
    arinputstream.cpp
    base64inputstream.cpp
    bz2inputstream.cpp
    cpioinputstream.cpp
    dataeventinputstream.cpp
    dostime.cpp
    encodinginputstream.cpp
    fileinputstream.cpp
    filestreamopener.cpp
    gzipinputstream.cpp
    kmpsearcher.cpp
    lzma/LzmaDec.c
    lzmainputstream.cpp
    mailinputstream.cpp
    oleinputstream.cpp
    rpminputstream.cpp
    sdfinputstream.cpp
    skippingfileinputstream.cpp
    stringterminatedsubstream.cpp
    subinputstream.cpp
    substreamproviderprovider.cpp
    tarinputstream.cpp
    textutils.cpp
    zipinputstream.cpp
    strigiconfig.cpp
    processinputstream.cpp
    mmapfileinputstream.cpp
    compat.cpp
)

add_library(streams SHARED ${streams_SRCS})

set_target_properties(streams PROPERTIES
    VERSION ${STRIGI_VERSION}
    SOVERSION ${STRIGI_SOVERSION}
)

target_link_libraries(streams
    ${ZLIB_LIBRARIES}
    ${BZIP2_LIBRARIES}
    ${ICONV_LIBRARIES}
)

install(TARGETS streams
    LIBRARY DESTINATION ${LIB_DESTINATION}
    ARCHIVE DESTINATION ${LIB_DESTINATION}
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libstreams.pc.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/libstreams.pc
    DESTINATION ${LIB_DESTINATION}/pkgconfig
)