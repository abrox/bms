
cmake_minimum_required (VERSION 2.8.11)
project (bms)

set (CMAKE_CXX_STANDARD 11)
add_subdirectory (tests)
add_subdirectory (lib)

include_directories ("${PROJECT_SOURCE_DIR}/lib")
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
add_custom_target(docs
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)

enable_testing ()
add_test (NAME MyTest COMMAND tests)


