MACRO(ADD_STRIGITA libname files)
    add_library(${libname} MODULE ${files})
    set_target_properties(${libname} PROPERTIES
        PREFIX strigita_
    )
    target_link_libraries(${libname} streamanalyzer)
    install(TARGETS ${libname} LIBRARY DESTINATION ${LIB_DESTINATION}/strigi)
ENDMACRO(ADD_STRIGITA)

if(XATTR_FOUND)
#  ADD_STRIGITA(xattr xattrthroughanalyzer.cpp)
endif ()

# Bug 164296: these two analyzers are dummy leftovers.They make sure that
# possible original, broken analyzers are overwritten/neutralized.
ADD_STRIGITA(avi dummy.cpp)
ADD_STRIGITA(wav dummy.cpp)

# disabled until a good check for making sure the file is a tga
#ADD_STRIGITA(tga  tgathroughanalyzer.cpp)
