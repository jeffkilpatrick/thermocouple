find_package(Phidget21 REQUIRED)

include_directories(
    ${PHIDGET21_INCLUDE_DIR}
    ${SRCDIR}/private
    ${SRCDIR}/public
)

set(SRC
    ${SRCDIR}/private/AbstractSensor.cpp
    ${SRCDIR}/private/MockSensor.cpp
    ${SRCDIR}/private/PhidgetsManager.cpp
    ${SRCDIR}/private/PhidgetsSensor.cpp
    ${SRCDIR}/private/SensorBroker.cpp
    ${SRCDIR}/private/SensorPoller.cpp
    ${SRCDIR}/private/Subscription.cpp
)

set(PRIVHDR
    ${SRCDIR}/private/PhidgetsSensor.h
)

set(PUBHDR
    ${SRCDIR}/public/AbstractSensor.h
    ${SRCDIR}/public/IListener.h
    ${SRCDIR}/public/ISensor.h
    ${SRCDIR}/public/MockSensor.h
    ${SRCDIR}/public/PhidgetsManager.h
    ${SRCDIR}/public/SensorBroker.h
    ${SRCDIR}/public/SensorPoller.h
    ${SRCDIR}/public/Subscription.h
)

add_library(Thermocouple SHARED
    ${SRC}
    ${PRIVHDR}
    ${PUBHDR}
)

target_link_libraries(Thermocouple
    ${PHIDGET21_LIBRARY}
)

set_target_properties(Thermocouple
    PROPERTIES
        FRAMEWORK true
        PUBLIC_HEADER "${PUBHDR}"
)

if(RUN_CLANG_TIDY)
    set_target_properties(Thermocouple PROPERTIES CXX_CLANG_TIDY "${RUN_CLANG_TIDY}")
endif()

install(TARGETS Thermocouple
    FRAMEWORK     DESTINATION /Library/Frameworks
    LIBRARY       DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
