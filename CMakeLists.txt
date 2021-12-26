cmake_minimum_required(VERSION 3.20)
project(chip_8_emulator)
set(EXE_TARGET chip-8_emulator)

set(CMAKE_CXX_STANDARD 20)

# SDL2
find_package(SDL2 REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/SDL2)

# Source Code
file(GLOB_RECURSE BIN_SRC
        CONFIGURE_DEPENDS
        src/*)

add_executable(${EXE_TARGET} ${BIN_SRC})

target_precompile_headers(${EXE_TARGET}
        PRIVATE
         src/stdafx.h
)

# SDL 2
target_include_directories(${EXE_TARGET} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${EXE_TARGET} PRIVATE ${SDL2_LIBRARIES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${EXE_TARGET} PRIVATE -DDEBUG)
endif()