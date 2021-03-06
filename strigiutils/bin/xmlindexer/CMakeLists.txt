add_library(librdfindexer STATIC
    rdfindexer.cpp
    tagmapping.cpp
    rdfindexwriter.cpp
)
target_link_libraries(librdfindexer
    streamanalyzer
    streams
)

add_library(libxmlindexer STATIC
    xmlindexer.cpp
    tagmapping.cpp
    xmlindexwriter.cpp
)
target_link_libraries(libxmlindexer
    streamanalyzer
    streams
)

add_executable(cgixmlindexer
    tagmapping.cpp
    xmlindexwriter.cpp
    cgixmlindexer.cpp
)
target_link_libraries(cgixmlindexer
    streamanalyzer
    streams
)

#add_executable(peranalyzerxml peranalyzerxml.cpp xmlindexwriter.cpp
#    tagmapping.cpp)
#target_link_libraries(peranalyzerxml streamanalyzer)

add_executable(perfieldxml
    perfieldxml.cpp xmlindexwriter.cpp
    tagmapping.cpp
)
target_link_libraries(perfieldxml
    streamanalyzer
    streams
)

add_executable(ontoprint ontoprint.cpp)
target_link_libraries(ontoprint streamanalyzer)

# register all tests based on the data in the testdata directory
#FILE(GLOB_RECURSE allfiles ../../testdata/analyzers/*/config)
FOREACH(file ${allfiles})
    GET_FILENAME_COMPONENT(testdir ${file} PATH)
    GET_FILENAME_COMPONENT(dir ${testdir} NAME)
    FILE(GLOB_RECURSE ofiles "${testdir}/*")
    FOREACH(ofile ${ofiles})
        STRING(REPLACE "/analyzers/${dir}/" "/data/" ifile ${ofile})
        IF(NOT ${ifile} MATCHES "config$")
            STRING(REPLACE "*/" "" testname ${ofile})
            STRING(REPLACE ${testdir} "" testname ${ofile})
            ADD_TEST("${dir}${testname}" peranalyzerxml -c ${testdir}/config -r ${ofile} ${ifile})
        ENDIF()
    ENDFOREACH(ofile ${ofiles})
ENDFOREACH(file ${allfiles})
