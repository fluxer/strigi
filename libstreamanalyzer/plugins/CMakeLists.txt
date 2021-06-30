macro(ADD_STRIGI_ANALYZER libname files)
    add_library(${libname} MODULE ${files})
    set_target_properties(${libname} PROPERTIES
        PREFIX strigi_
    )
    target_link_libraries(${libname} streamanalyzer)
    install(
        TARGETS ${libname}
        LIBRARY DESTINATION ${LIB_DESTINATION}/strigi
    )
endmacro()

add_subdirectory(endplugins)
add_subdirectory(throughplugins)
