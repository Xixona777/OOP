cmake_minimum_required(VERSION 3.20)

project(DungeonEditor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(dungeon_lib
    src/BattleVisitor.cpp
    src/Dungeon.cpp
    src/Npc.cpp
    src/NpcFactory.cpp
    src/Observers.cpp
)
target_include_directories(dungeon_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(dungeon_lib PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(dungeon_lib PRIVATE /W4 /permissive-)
else()
    target_compile_options(dungeon_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(dungeon_editor main.cpp)
target_link_libraries(dungeon_editor PRIVATE dungeon_lib)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(dungeon_tests tests/DungeonTests.cpp)
    target_link_libraries(dungeon_tests PRIVATE dungeon_lib GTest::gtest_main)
    if(MSVC)
        target_compile_options(dungeon_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(dungeon_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    include(GoogleTest)
    gtest_discover_tests(dungeon_tests DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)
endif()
