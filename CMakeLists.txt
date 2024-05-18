cmake_minimum_required(VERSION 3.27)
project(recruitment_test)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR})

# Add your source files and the Google Test main file
add_executable(recruitment_test
        task.cpp
        Computer_Club_STRUCTS.h
        Computer_Club.cpp
        Computer_Club.h
        helper_functions.h
        helper_functions.cpp
        parsing_functions.h
        parsing_functions.cpp
)

# Link Google Test to your test executable along with any other necessary libraries
target_link_libraries(recruitment_test ${GTEST_LIBRARIES} pthread)

# Add your tests (if you have test files, list them here)
add_executable(test_PARSING tests/test_PARSING.cpp parsing_functions.cpp)
add_executable(test_HELPERS tests/test_HELPRES.cpp helper_functions.cpp)
add_executable(test_COMPUTER_CLUB tests/test_COMPUTER_CLUB.cpp Computer_Club.cpp helper_functions.cpp parsing_functions.cpp)
target_link_libraries(test_PARSING ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_HELPERS ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_COMPUTER_CLUB ${GTEST_LIBRARIES} pthread)

# Add the test to be run by CTest
add_test(NAME TestParsingFuncs COMMAND test_PARSING)
add_test(NAME TestHelperFuncs COMMAND test_HELPERS)
add_test(NAME TestComputerClub COMMAND test_COMPUTER_CLUB)