include_directories(
	${SRCDIR}/private
	${SRCDIR}/public
)

set(SRC
	${SRCDIR}/PhidgetsMonitor/main.cpp
)

add_executable(PhidgetsMonitor
	${SRC}
)

target_link_libraries(PhidgetsMonitor
	Thermocouple
)
