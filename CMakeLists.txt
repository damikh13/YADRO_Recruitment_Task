cmake_minimum_required(VERSION 3.27)
project(recruitment_test)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})

add_executable(computer_club
        task.cpp
        Computer_Club_STRUCTS.h
        Computer_Club.cpp
        Computer_Club.h
        helper_functions.h
        helper_functions.cpp
        parsing_functions.h
        parsing_functions.cpp
)

target_link_libraries(computer_club ${GTEST_LIBRARIES} pthread)

add_executable(test_PARSING tests/test_PARSING.cpp parsing_functions.cpp)
add_executable(test_HELPERS tests/test_HELPRES.cpp helper_functions.cpp)
target_link_libraries(test_PARSING ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_HELPERS ${GTEST_LIBRARIES} pthread)

# CTest
add_test(NAME TestParsingFuncs COMMAND test_PARSING)
add_test(NAME TestHelperFuncs COMMAND test_HELPERS)