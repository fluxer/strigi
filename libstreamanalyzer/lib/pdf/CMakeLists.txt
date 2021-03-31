add_executable(pdftest pdftest.cpp)
target_link_libraries(pdftest streams ${FILE_LIBRARIES})

add_executable(pdf pdf.cpp pdfparser.cpp)
target_link_libraries(pdf streams)
