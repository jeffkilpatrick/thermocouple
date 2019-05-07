cmake_minimum_required(VERSION 3.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")

project(Thermocouple C CXX)

# https://gitlab.kitware.com/cmake/cmake/issues/15943
if(POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)

if(UNIX)
    add_compile_options(-Werror -Wall -Wno-unknown-pragmas)
    if(APPLE)
        add_compile_options(-stdlib=libc++)
        add_link_options(-stdlib=libc++)
    endif()
endif()


if(DEFINED ENV{CLANG_TIDY})
    find_program(
        CLANG_TIDY_EXE
        NAMES "clang-tidy"
        PATHS "/usr/local/opt/llvm/bin")

    if(CLANG_TIDY_EXE)
        set(RUN_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-*-magic-numbers,-clang-analyzer-alpha.*,-cppcoreguidelines-pro-type-reinterpret-cast,-fuchsia-*,-google-readability-todo,-*-non-private-member-variables-in-classes")
    endif()
endif()

get_filename_component(ROOTDIR ${CMAKE_SOURCE_DIR}/.. ABSOLUTE)
set(SRCDIR ${ROOTDIR}/src)
set(TESTDIR ${ROOTDIR}/test)

add_subdirectory(Thermocouple)
add_subdirectory(PhidgetsMonitor)
add_subdirectory(ThermocoupleMonitor)
add_subdirectory(gtest)
add_subdirectory(unittest)
