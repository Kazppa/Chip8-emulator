cmake_minimum_required(VERSION 3.20)
project(chip_8_emulator)
set(CHIP8_EXE chip-8_emulator)

set(CMAKE_CXX_STANDARD 20)

# SDL2
find_package(SDL2 REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2")

# Source Code
file(GLOB_RECURSE BIN_SRC
        CONFIGURE_DEPENDS
        src/*)

add_executable(${CHIP8_EXE} "${BIN_SRC}")

target_include_directories(${CHIP8_EXE} PRIVATE include)

target_precompile_headers(${CHIP8_EXE}
        PRIVATE
        src/stdafx.h
        )

# SDL 2
target_include_directories(${CHIP8_EXE} PRIVATE "${SDL2_INCLUDE_DIRS}")
target_link_libraries(${CHIP8_EXE} PRIVATE "${SDL2_LIBRARIES}")

# Compile options
if (MSVC)
    target_compile_options(${CHIP8_EXE} PRIVATE
            "/W4" "$<$<CONFIG:RELEASE>:/O2>")
else ()
    target_compile_options(${CHIP8_EXE} PRIVATE
            "-Wall" "-Wextra" -"Werror" "$<$<CONFIG:Release>:-03>")
endif ()

# DEBUG macro
target_compile_definitions(${CHIP8_EXE} PRIVATE
        $<$<CONFIG:Debug>:
        DEBUG
        >)

add_custom_command(
        TARGET ${CHIP8_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2/lib/x64/SDL2.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/")