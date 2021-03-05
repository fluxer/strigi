MACRO(ADD_STRIGILA libname files)
    add_library(${libname} MODULE ${files})
    set_target_properties(${libname} PROPERTIES
        PREFIX strigila_
    )
    target_link_libraries(${libname} streamanalyzer)
    install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGILA)

ADD_STRIGILA(xpm xpmlineanalyzer.cpp)
ADD_STRIGILA(deb deblineanalyzer.cpp)
ADD_STRIGILA(cpp cpplineanalyzer.cpp)
ADD_STRIGILA(txt txtlineanalyzer.cpp)
