cmake_minimum_required(VERSION 3.27)
project(recruitment_test)

set(CMAKE_CXX_STANDARD 20)

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
