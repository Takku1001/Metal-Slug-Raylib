cmake_minimum_required(VERSION 3.16)
project(MetalSlugRaylib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- raylib (provided via vcpkg or a system install) ---
find_package(raylib CONFIG REQUIRED)

# --- Game executable ---
file(GLOB GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_executable(metal_slug ${GAME_SOURCES})
target_include_directories(metal_slug PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(metal_slug PRIVATE raylib)

# Assets are loaded by relative filename, so copy them next to the binary.
add_custom_command(TARGET metal_slug POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:metal_slug>"
    COMMENT "Copying assets next to the executable")

# --- Headless core-logic tests ---
option(BUILD_TESTS "Build the headless core-logic tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_executable(core_tests
        tests/test_core.cpp
        src/Bullet.cpp
        src/Player.cpp)
    target_include_directories(core_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
    target_link_libraries(core_tests PRIVATE raylib)
    add_test(NAME core_tests COMMAND core_tests)
endif()
