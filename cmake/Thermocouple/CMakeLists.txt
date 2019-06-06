find_package(Phidget21)

include_directories(
    ${SRCDIR}/private
    ${SRCDIR}/public
)

set(SRC
    ${SRCDIR}/private/AbstractSensor.cpp
    ${SRCDIR}/private/MockSensor.cpp
    ${SRCDIR}/private/SensorBroker.cpp
    ${SRCDIR}/private/SensorPoller.cpp
    ${SRCDIR}/private/Subscription.cpp
)

set(PUBHDR
    ${SRCDIR}/public/AbstractSensor.h
    ${SRCDIR}/public/IListener.h
    ${SRCDIR}/public/ISensor.h
    ${SRCDIR}/public/MockSensor.h
    ${SRCDIR}/public/SensorBroker.h
    ${SRCDIR}/public/SensorPoller.h
    ${SRCDIR}/public/Subscription.h
)

if(Phidget21_FOUND)
    include_directories(
        ${PHIDGET21_INCLUDE_DIR}
    )

    list(APPEND SRC
        ${SRCDIR}/private/PhidgetsManager.cpp
        ${SRCDIR}/private/PhidgetsSensor.cpp
    )

    list(APPEND PUBHDR
        ${SRCDIR}/public/PhidgetsManager.h
    )

    set(PRIVHDR
        ${SRCDIR}/private/PhidgetsSensor.h
    )
endif()

add_library(Thermocouple SHARED
    ${SRC}
    ${PRIVHDR}
    ${PUBHDR}
)

if(Phidget21_FOUND)
    target_link_libraries(Thermocouple
        ${PHIDGET21_LIBRARY}
    )
endif()

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
