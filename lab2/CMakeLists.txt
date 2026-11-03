cmake_minimum_required(VERSION 3.14)
project(ThreeLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(three Three.cpp)
target_include_directories(three PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(three_demo main.cpp)
target_link_libraries(three_demo PRIVATE three)

include(FetchContent)
find_package(GTest CONFIG QUIET)
if(NOT GTest_FOUND)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)
endif()

enable_testing()
add_executable(three_tests test_three.cpp)
target_link_libraries(three_tests PRIVATE three GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(three_tests)
