cmake_minimum_required(VERSION 3.20)

project(oop_lab5_iterators_allocators VERSION 1.0.0 LANGUAGES CXX)

option(LAB5_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

add_library(lab5_core STATIC
    src/fixed_block_memory_resource.cpp
)
target_include_directories(lab5_core PUBLIC include)
target_compile_features(lab5_core PUBLIC cxx_std_20)
set_target_properties(lab5_core PROPERTIES CXX_EXTENSIONS OFF)

if(MSVC)
    target_compile_options(lab5_core PUBLIC /W4 /permissive-)
else()
    target_compile_options(lab5_core PUBLIC -Wall -Wextra -Wpedantic)
endif()

if(LAB5_ENABLE_SANITIZERS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(lab5_core PUBLIC
            -fsanitize=address,undefined
            -fno-omit-frame-pointer
        )
        target_link_options(lab5_core INTERFACE -fsanitize=address,undefined)
    else()
        message(FATAL_ERROR "Sanitizers are configured only for GCC and Clang")
    endif()
endif()

add_executable(lab5_demo main.cpp)
target_link_libraries(lab5_demo PRIVATE lab5_core)
set_target_properties(lab5_demo PROPERTIES CXX_EXTENSIONS OFF)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(lab5_tests
        tests/fixed_block_memory_resource_test.cpp
        tests/singly_linked_list_test.cpp
    )
    target_link_libraries(lab5_tests PRIVATE lab5_core GTest::gtest_main)
    set_target_properties(lab5_tests PROPERTIES CXX_EXTENSIONS OFF)

    include(GoogleTest)
    gtest_discover_tests(lab5_tests DISCOVERY_TIMEOUT 30)
endif()
