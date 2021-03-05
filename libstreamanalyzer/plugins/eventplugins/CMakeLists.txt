MACRO(ADD_STRIGIEA libname files)
    add_library(${libname} MODULE ${files})
    set_target_properties(${libname} PROPERTIES
        PREFIX strigiea_
    )
    target_link_libraries(${libname} streamanalyzer)
    install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGIEA)

include_directories(.)

ADD_STRIGIEA(riff riffeventanalyzer.cpp)
ADD_STRIGIEA(digest digesteventanalyzer.cpp)
