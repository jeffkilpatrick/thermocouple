find_package(ncurses REQUIRED)

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

target_link_libraries(ThermocoupleMonitor
    Thermocouple
    ${NCURSES_LIBRARY}
)

install(TARGETS ThermocoupleMonitor
    RUNTIME DESTINATION bin)
