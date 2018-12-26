
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable (tests 
                main.cpp 
                testbattery.cpp
                ${PROJECT_SOURCE_DIR}/lib/battery.cpp
               )
find_library(UNIT_TEST_LIB libUnitTest++.a )
message(STATUS ${UNIT_TEST_LIB})
target_link_libraries (tests ${UNIT_TEST_LIB})

