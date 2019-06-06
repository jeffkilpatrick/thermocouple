find_package(ncurses REQUIRED)
find_package(Phidget21)
find_package(Threads REQUIRED)

include_directories(
    ${NCURSES_INCLUDE_DIR}
    ${SRCDIR}/public
)

set(SRC
    ${SRCDIR}/ThermocoupleMonitor/main.cpp
)

add_executable(ThermocoupleMonitor
    ${SRC}
)

if(Phidget21_FOUND)
    add_definitions(-DHAVE_PHIDGETS21)
endif()

target_link_libraries(ThermocoupleMonitor
    Thermocouple
    ${NCURSES_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS ThermocoupleMonitor
    RUNTIME DESTINATION bin)
