include(gtest)

find_package(Threads REQUIRED)

include_directories(
    ${GTESTDIR}/include
    ${GTESTDIR}
)

add_library(gtest STATIC
    ${GTESTDIR}/src/gtest-all.cc
)

target_link_libraries(gtest
    ${CMAKE_THREAD_LIBS_INIT}
)
