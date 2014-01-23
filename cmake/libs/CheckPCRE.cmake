# Check for PCRE.
# If PCRE isn't found, extlib/pcre/ will be used instead.
IF(NOT HAVE_PCRE)

IF(WIN32)
	MESSAGE(STATUS "Win32: using internal PCRE")
ELSEIF(USE_INTERNAL_PNG)
	MESSAGE(STATUS "Using internal PCRE")
ELSE()
	FIND_PACKAGE(PCRE)
	IF(NOT PCRE_FOUND)
		MESSAGE(STATUS "PCRE not found; using internal PCRE")
	ENDIF(NOT PCRE_FOUND)
ENDIF()
SET(HAVE_PCRE ${PCRE_FOUND})

IF(NOT PCRE_FOUND)
	# PCRE wasn't found.
	# Use the internal PCRE.
	SET(USE_INTERNAL_PCRE 1)
	SET(PCRE_PCRE_LIBRARY pcre)
	SET(PCRE_FOUND 1)
	SET(HAVE_PCRE 1)
	SET(PCRE_STATIC 1)
	SET(PCRE_DEFINITIONS ${PCRE_DEFINITIONS} -DPCRE_STATIC)
	SET(PCRE_INCLUDE_DIR
		"${CMAKE_CURRENT_SOURCE_DIR}/extlib/pcre/"
		"${CMAKE_CURRENT_BINARY_DIR}/extlib/pcre/"
		)
ENDIF(NOT PCRE_FOUND)

ENDIF(NOT HAVE_PCRE)
