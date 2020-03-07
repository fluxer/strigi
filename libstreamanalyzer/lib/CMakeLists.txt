set(streamanalyzer_SRCS
	analysisresult.cpp
	analyzerconfiguration.cpp
	analyzerloader.cpp
	classproperties.cpp
	diranalyzer.cpp
	eventthroughanalyzer.cpp
	fieldproperties.cpp
	fieldpropertiesdb.cpp
	fieldtypes.cpp
	filelister.cpp
	fnmatch.cpp
	indexpluginloader.cpp
	lineeventanalyzer.cpp
	pdf/pdfparser.cpp
	query.cpp
	queryparser.cpp
	saxeventanalyzer.cpp
	streamanalyzer.cpp
	streamanalyzerfactory.cpp
	streamsaxanalyzer.cpp
	throughanalyzers/oggthroughanalyzer.cpp
	variant.cpp
        indexreader.cpp
	endanalyzers/arendanalyzer.cpp
	endanalyzers/bmpendanalyzer.cpp
	endanalyzers/bz2endanalyzer.cpp
	endanalyzers/cpioendanalyzer.cpp
	endanalyzers/flacendanalyzer.cpp
	endanalyzers/gzipendanalyzer.cpp
	endanalyzers/id3endanalyzer.cpp
	endanalyzers/lzmaendanalyzer.cpp
	endanalyzers/mailendanalyzer.cpp
	endanalyzers/mpegendanalyzer.cpp
	endanalyzers/odfendanalyzer.cpp
	endanalyzers/oleendanalyzer.cpp
	endanalyzers/pdfendanalyzer.cpp
	endanalyzers/pngendanalyzer.cpp
	endanalyzers/rpmendanalyzer.cpp
	endanalyzers/sdfendanalyzer.cpp
	endanalyzers/tarendanalyzer.cpp
	endanalyzers/textendanalyzer.cpp
	endanalyzers/zipendanalyzer.cpp
	endanalyzers/zipexeendanalyzer.cpp
	eventanalyzers/mimeeventanalyzer.cpp
	helperanalyzers/odfcontenthelperanalyzer.cpp
	helperanalyzers/odfmetahelperanalyzer.cpp
	helperanalyzers/saxhelperanalyzer.cpp
	lineanalyzers/m3ustreamanalyzer.cpp
	saxanalyzers/htmlsaxanalyzer.cpp
	xmlparser/libxmlstream.cpp
	xmlparser/xmlstream.cpp
	xesamparser.cpp
)

#add the compatibility files
set(streamanalyzer_SRCS ${streamanalyzer_SRCS} compat.cpp)

if(MSVC)
	list(APPEND streamanalyzer_SRCS
		stgdirent.cpp
		stgdirent.h
	)
else()
	list(APPEND streamanalyzer_SRCS
		endanalyzers/helperendanalyzer.cpp
	)
endif()

add_library(streamanalyzer SHARED ${streamanalyzer_SRCS})
set_target_properties(streamanalyzer PROPERTIES
	VERSION ${LIBSTREAMANALYZER_VERSION}
	SOVERSION ${LIBSTREAMANALYZER_SOVERSION}
)
target_link_libraries(streamanalyzer
    ${LIBSTREAMS_LIBRARIES}
    ${LIBXML2_LIBRARIES}
    ${ICONV_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${FILE_LIBRARIES}
)

install(TARGETS streamanalyzer
	LIBRARY DESTINATION ${LIB_DESTINATION}
	RUNTIME DESTINATION bin
	ARCHIVE DESTINATION ${LIB_DESTINATION}
)