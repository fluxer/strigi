project (strigiutils)

##### building and testing #####

include_directories(
    ${libstreams_SOURCE_DIR}/include
    ${libstreams_BINARY_DIR}/include
    ${libstreamanalyzer_SOURCE_DIR}/include
    ${libstreamanalyzer_BINARY_DIR}/include
)

add_subdirectory(bin)

##### installing #####

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp "")

add_executable(deepfind dummy.cpp)
target_link_libraries(deepfind libdeepfind)
install(TARGETS deepfind
    RUNTIME DESTINATION ${BIN_DESTINATION}
)

if(HAVE_REGEX_H)
    add_executable(deepgrep dummy.cpp)
    target_link_libraries(deepgrep libdeepgrep)
    install(TARGETS deepgrep
        RUNTIME DESTINATION ${BIN_DESTINATION}
    )
endif(HAVE_REGEX_H)

add_executable(xmlindexer dummy.cpp)
target_link_libraries(xmlindexer libxmlindexer)
install(TARGETS xmlindexer
    RUNTIME DESTINATION ${BIN_DESTINATION}
)

add_executable(rdfindexer dummy.cpp)
target_link_libraries(rdfindexer librdfindexer)
install(TARGETS rdfindexer
    RUNTIME DESTINATION ${BIN_DESTINATION}
)
