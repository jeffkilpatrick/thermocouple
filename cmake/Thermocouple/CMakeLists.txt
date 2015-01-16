find_package(Phidget21 REQUIRED)
include_directories(${PHIDGET21_INCLUDE_DIR})

include_directories(
	${SRCDIR}/private
	${SRCDIR}/public
)

set(SRC
	${SRCDIR}/private/AbstractSensor.cpp
	${SRCDIR}/private/PhidgetsManager.cpp
	${SRCDIR}/private/PhidgetsSensor.cpp
	${SRCDIR}/private/PollingSensor.cpp
	${SRCDIR}/private/SensorBroker.cpp
	${SRCDIR}/private/Subscription.cpp
)

set(PRIVHDR
	${SRCDIR}/private/AbstractSensor.h
	${SRCDIR}/private/InitOnce.h
	${SRCDIR}/private/PhidgetsSensor.h
	${SRCDIR}/private/PollingSensor.h
)

set(PUBHDR
	${SRCDIR}/public/IListener.h
	${SRCDIR}/public/ISensor.h
	${SRCDIR}/public/PhidgetsManager.h
	${SRCDIR}/public/SensorBroker.h
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

install(TARGETS Thermocouple
    LIBRARY       DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)