MACRO(ADD_STRIGITA libname files)
    add_library(${libname} MODULE ${files})
    set_target_properties(${libname} PROPERTIES
        PREFIX strigita_
    )
    target_link_libraries(${libname} streamanalyzer)
    install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGITA)

if(XATTR_FOUND)
    # include_directories(${XATTR_INCLUDE_DIR})
    # ADD_STRIGITA(xattr xattrthroughanalyzer.cpp)
endif()

# disabled until a good check for making sure the file is a tga
#ADD_STRIGITA(tga  tgathroughanalyzer.cpp)
