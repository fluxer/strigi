set(streamtests
    testrunner.cpp
    Base64InputStreamTest.cpp
    SkippingFileInputStreamTest.cpp
    MMapFileInputStreamTest.cpp
)

create_test_sourcelist(TESTS ${streamtests})

add_executable(testrunner-libstreams-internal ${TESTS}
    ../sharedtestcode/inputstreamtests.cpp
)
target_link_libraries(testrunner-libstreams-internal streams)

set(TESTSTORUN ${TESTS})
list(REMOVE_ITEM TESTSTORUN testrunner.cpp)

foreach(TESTTORUN ${TESTSTORUN})
    get_filename_component(TESTNAME ${TESTTORUN} NAME_WE)
    add_test(${TESTNAME} testrunner-libstreams-internal
        ${TESTNAME} ${libstreams_SOURCE_DIR}/tests/data
    )
endforeach()
