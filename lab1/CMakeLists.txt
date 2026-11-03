cmake_minimum_required(VERSION 3.20)

project(lab1_vowels LANGUAGES CXX)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
   NOT CMAKE_CXX_COMPILER_VERSION MATCHES "^14\\.")
    message(FATAL_ERROR
        "This project requires GCC 14. Detected: "
        "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(vowels src/vowels.cpp)
target_include_directories(vowels PUBLIC include)

add_executable(vowels_app main.cpp)
target_link_libraries(vowels_app PRIVATE vowels)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(vowels_tests tests/vowels_test.cpp)
    target_link_libraries(vowels_tests PRIVATE vowels GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(vowels_tests)
endif()
