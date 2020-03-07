add_executable(dummyindexer dummyindexer.cpp)
target_link_libraries(dummyindexer ${LIBSTREAMANALYZER_LIBRARIES})

add_library(libdeepfind STATIC deepfind.cpp)
target_link_libraries(libdeepfind ${LIBSTREAMANALYZER_LIBRARIES})

if(HAVE_REGEX_H)
    add_library(libdeepgrep STATIC deepgrep.cpp grepindexreader.cpp grepindexmanager.cpp
        grepindexwriter.cpp)
    target_link_libraries(libdeepgrep ${LIBSTREAMANALYZER_LIBRARIES})
endif(HAVE_REGEX_H)

add_executable(greptest grepindexreader.cpp)
target_link_libraries(greptest streamanalyzer)

if(NOT MSVC)
  add_executable(analyzerlatencytester analyzerlatencytester.cpp)
  target_link_libraries(analyzerlatencytester ${LIBSTREAMANALYZER_LIBRARIES})
endif(NOT MSVC)

add_library(grepindex STATIC grepindexmanager.cpp)
