cmake_minimum_required(VERSION 3.24)

project(lab7_async VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TSAN "Build with ThreadSanitizer" OFF)

function(lab7_copy_mingw_runtime target)
    if(MINGW)
        get_filename_component(mingw_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
        foreach(runtime_dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
            if(EXISTS "${mingw_bin}/${runtime_dll}")
                add_custom_command(
                    TARGET ${target} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            "${mingw_bin}/${runtime_dll}"
                            "$<TARGET_FILE_DIR:${target}>/${runtime_dll}"
                )
            endif()
        endforeach()
    endif()
endfunction()

if(ENABLE_TSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=thread -fno-omit-frame-pointer)
        add_link_options(-fsanitize=thread)
    else()
        message(FATAL_ERROR "ThreadSanitizer is not supported by this compiler")
    endif()
endif()

find_package(Threads REQUIRED)

add_library(lab7_core
    src/battle_queue.cpp
    src/battle_task.cpp
    src/combat.cpp
    src/dice.cpp
    src/map_renderer.cpp
    src/npc.cpp
    src/npc_factory.cpp
    src/observer.cpp
    src/simulation.cpp
    src/world.cpp
)
target_include_directories(lab7_core PUBLIC include)
target_link_libraries(lab7_core PUBLIC Threads::Threads)

if(MSVC)
    target_compile_options(lab7_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(lab7_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(lab7_async main.cpp)
target_link_libraries(lab7_async PRIVATE lab7_core)
lab7_copy_mingw_runtime(lab7_async)

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

    add_executable(lab7_tests
        tests/combat_tests.cpp
        tests/npc_tests.cpp
        tests/simulation_tests.cpp
        tests/world_tests.cpp
    )
    target_link_libraries(lab7_tests PRIVATE lab7_core GTest::gtest_main)
    target_include_directories(lab7_tests PRIVATE tests)
    lab7_copy_mingw_runtime(lab7_tests)

    include(GoogleTest)
    gtest_discover_tests(lab7_tests DISCOVERY_MODE PRE_TEST)
endif()
