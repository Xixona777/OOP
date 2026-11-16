cmake_minimum_required(VERSION 3.24)
project(oop_lab4 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(figures INTERFACE)
target_include_directories(figures INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(demo_lib src/Demo.cpp)
target_link_libraries(demo_lib PUBLIC figures)

add_executable(oop_lab4_demo main.cpp)
target_link_libraries(oop_lab4_demo PRIVATE demo_lib)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(oop_lab4_tests tests/test_figures.cpp tests/test_array.cpp)
    target_link_libraries(oop_lab4_tests PRIVATE figures GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(oop_lab4_tests)
endif()
