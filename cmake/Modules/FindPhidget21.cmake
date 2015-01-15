find_path(
	PHIDGET21_INCLUDE_DIR phidget21.h
	HINTS /usr/include /Library/Frameworks
)

find_library(
	PHIDGET21_LIBRARY NAMES phidget21 libphidget21
     HINTS /usr/lib /Library/Frameworks
 )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
	Phidget21  DEFAULT_MSG
    PHIDGET21_LIBRARY PHIDGET21_INCLUDE_DIR)

 mark_as_advanced(${PHIDGET21_INCLUDE_DIR} ${PHIDGET21_LIBRARY})
