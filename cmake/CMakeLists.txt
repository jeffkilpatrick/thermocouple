cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")
MESSAGE(${CMAKE_MODULE_PATH})

if(UNIX)
	set(CXX_WARNINGS -Werror -Wall)
	if(APPLE)
		set(CXX_EXTRA_FLAGS -stdlib=libc++)
	endif()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${CXX_EXTRA_FLAGS}")
endif()

get_filename_component(ROOTDIR ${CMAKE_SOURCE_DIR}/.. ABSOLUTE)
set(SRCDIR ${ROOTDIR}/src)

add_subdirectory(Thermocouple)
add_subdirectory(PhidgetsMonitor)
