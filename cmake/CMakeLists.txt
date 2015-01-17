cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")

project(Thermocouple)

if(UNIX)
    set(CXX_WARNINGS "-Werror -Wall -Wno-unknown-pragmas")
    if(APPLE)
        set(CXX_EXTRA_FLAGS "-std=c++11 -stdlib=libc++")
    else()
        set(CXX_EXTRA_FLAGS "-std=c++11")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_EXTRA_FLAGS} ${CXX_WARNINGS}")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.7.999")
        MESSAGE(FATAL_ERROR "Version 4.8+ required when using g++")
    endif()
endif()

get_filename_component(ROOTDIR ${CMAKE_SOURCE_DIR}/.. ABSOLUTE)
set(SRCDIR ${ROOTDIR}/src)

add_subdirectory(Thermocouple)
add_subdirectory(PhidgetsMonitor)
add_subdirectory(ThermocoupleMonitor)