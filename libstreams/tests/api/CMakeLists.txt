set(streamtests
    testrunner.cpp
    ArInputStreamTest.cpp
    BZ2InputStreamTest.cpp
    CpioInputStreamTest.cpp
    EventInputStreamTest.cpp
    FileInputStreamTest.cpp
    GZipInputStreamTest.cpp
    InputStreamTest.cpp
    KmpSearcherTest.cpp
    LZMAInputStreamTest.cpp
    MailInputStreamTest.cpp
    OleInputStreamTest.cpp
    RpmInputStreamTest.cpp
    SdfInputStreamTest.cpp
    StringTerminatedSubStreamTest.cpp
    SubInputStreamTest.cpp
    TarInputStreamTest.cpp
    ZipInputStreamTest.cpp
    ArchiveReaderTest.cpp
    ProcessInputStreamTest.cpp
    StringStreamTest.cpp # this maybe works with mingw
)

create_test_sourcelist(Tests ${streamtests})

add_executable(testrunner-streams ${Tests}
    ../sharedtestcode/inputstreamtests.cpp
)
target_link_libraries(testrunner-streams streams)

set(TestsToRun ${Tests})
remove(TestsToRun testrunner.cpp)

foreach(test ${TestsToRun})
    get_filename_component(TName ${test} NAME_WE)
    add_test(${TName} testrunner-streams ${TName} ${libstreams_SOURCE_DIR}/tests/data)
endforeach()
