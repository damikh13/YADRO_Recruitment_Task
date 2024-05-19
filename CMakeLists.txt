cmake_minimum_required(VERSION 3.27)
project(recruitment_test)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

include(FetchContent)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.11.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_BINARY_DIR})

# Main executable
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

# Test executables
add_executable(test_PARSING tests/test_PARSING.cpp parsing_functions.cpp)
add_executable(test_HELPERS tests/test_HELPRES.cpp helper_functions.cpp)

# Link libraries
target_link_libraries(computer_club pthread)
target_link_libraries(test_PARSING gtest gtest_main pthread)
target_link_libraries(test_HELPERS gtest gtest_main pthread)

# CTest
add_test(NAME TestParsingFuncs COMMAND test_PARSING)
add_test(NAME TestHelperFuncs COMMAND test_HELPERS)
