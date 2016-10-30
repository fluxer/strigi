project (strigi)

# This is the aggregate build file. It serves only to call the build in the
# subdirectories. Each of the projects in the subdirectories is buildable
# separately.

cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)
# for testing to work in cmake, this command must be called in the root src dir
enable_testing()

add_subdirectory(libstreams) # Must be first
add_subdirectory(libstreamanalyzer) # Must be second
add_subdirectory(strigiutils)

