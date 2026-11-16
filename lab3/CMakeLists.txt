cmake_minimum_required(VERSION 3.20)

project(oop_lab3_variant6 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(figures
    src/figure.cpp
    src/pentagon.cpp
    src/hexagon.cpp
    src/octagon.cpp
    src/figure_array.cpp
)

target_include_directories(figures PUBLIC include)

if(MSVC)
    target_compile_options(figures PRIVATE /W4 /permissive-)
else()
    target_compile_options(figures PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(lab3_demo src/main.cpp)
target_link_libraries(lab3_demo PRIVATE figures)

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

    add_executable(lab3_tests
        tests/test_figures.cpp
        tests/test_array.cpp
    )
    target_link_libraries(lab3_tests PRIVATE figures GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(lab3_tests)
endif()
