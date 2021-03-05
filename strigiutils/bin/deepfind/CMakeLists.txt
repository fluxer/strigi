add_executable(dummyindexer dummyindexer.cpp)
target_link_libraries(dummyindexer streamanalyzer)

add_library(libdeepfind STATIC deepfind.cpp)
target_link_libraries(libdeepfind streamanalyzer)

if(HAVE_REGEX_H)
    add_library(libdeepgrep STATIC deepgrep.cpp grepindexreader.cpp grepindexmanager.cpp
        grepindexwriter.cpp)
    target_link_libraries(libdeepgrep streamanalyzer)
endif(HAVE_REGEX_H)

add_executable(greptest grepindexreader.cpp)
target_link_libraries(greptest streamanalyzer)

add_executable(analyzerlatencytester analyzerlatencytester.cpp)
target_link_libraries(analyzerlatencytester streamanalyzer)

add_library(grepindex STATIC grepindexmanager.cpp)
