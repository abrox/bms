

find_package(UnitTest++ REQUIRED)
include_directories(${UTPP_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable (tests 
                main.cpp 
                testbattery.cpp
                testui.cpp
                ${PROJECT_SOURCE_DIR}/lib/battery.cpp
               )
target_link_libraries (tests UnitTest++)



