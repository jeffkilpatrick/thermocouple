find_path(
    NCURSES_INCLUDE_DIR ncurses.h
    PATHS /usr/include /usr/local/include
)

find_library(
    NCURSES_LIBRARY NAMES ncurses ncurses
    PATHS /usr/lib /usr/local/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    ncurses  DEFAULT_MSG
    NCURSES_LIBRARY NCURSES_INCLUDE_DIR
)

mark_as_advanced(${NCURSES_INCLUDE_DIR} ${NCURSES_LIBRARY})
