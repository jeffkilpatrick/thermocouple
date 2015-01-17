include(gtest)

include_directories(
    ${SRCDIR}/private
    ${SRCDIR}/public
    ${GTESTDIR}/include
)

set(SRC
    ${TESTDIR}/MockSensor.cpp
    ${TESTDIR}/MockSensorTest.cpp
    ${TESTDIR}/RecordingListener.cpp
    ${TESTDIR}/SensorBrokerTest.cpp
)

set(PRIVHDR
    ${TESTDIR}/MockSensor.h
    ${TESTDIR}/RecordingListener.h
)

add_executable(unittest
    ${SRC}
    ${PRIVHDR}
    ${GTESTDIR}/src/gtest_main.cc
)

target_link_libraries(unittest
    Thermocouple
    gtest
)