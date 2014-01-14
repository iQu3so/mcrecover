# Platform-specific functionality.

# Hack to remove -rdynamic from CFLAGS and CXXFLAGS
# See http://public.kitware.com/pipermail/cmake/2006-July/010404.html
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)
	SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
ENDIF()

# Don't embed rpaths in the executables.
SET(CMAKE_SKIP_RPATH ON)

IF(MSVC)
	INCLUDE(cmake/platform/msvc.cmake)
ELSE(MSVC)
	INCLUDE(cmake/platform/gcc.cmake)
ENDIF(MSVC)

# Platform-specific configuration.
SET(MCRECOVER_CFLAGS_PLATFORM "")
SET(MCRECOVER_CXXFLAGS_PLATFORM "")
SET(MCRECOVER_LDFLAGS_PLATFORM "")
IF(WIN32)
	INCLUDE(cmake/platform/win32.cmake)
ENDIF(WIN32)

# Common CFLAGS/CXXFLAGS for each configuration.
SET(MCRECOVER_CFLAGS_CONFIG_COMMON	"${MCRECOVER_C99_CFLAG} ${MCRECOVER_CFLAGS_COMMON} ${MCRECOVER_CFLAGS_PLATFORM}")
SET(MCRECOVER_CXXFLAGS_CONFIG_COMMON	"${MCRECOVER_CXX11_CXXFLAG} ${MCRECOVER_CXXFLAGS_COMMON} ${MCRECOVER_CXXFLAGS_PLATFORM}")
SET(MCRECOVER_LDFLAGS_CONFIG_COMMON	"${MCRECOVER_LDFLAGS_COMMON} ${MCRECOVER_LDFLAGS_PLATFORM}")

# Set CFLAGS/CXXFLAGS based on build type.
SET(CMAKE_C_FLAGS_DEBUG			"${MCRECOVER_CFLAGS_COMPILER_DEBUG} -DDEBUG ${MCRECOVER_CFLAGS_CONFIG_COMMON}")
SET(CMAKE_CXX_FLAGS_DEBUG		"${MCRECOVER_CXXFLAGS_COMPILER_DEBUG} -DDEBUG ${MCRECOVER_CXXFLAGS_CONFIG_COMMON}")
SET(CMAKE_C_FLAGS_RELEASE		"${MCRECOVER_CFLAGS_COMPILER_RELEASE} -DNDEBUG ${MCRECOVER_CFLAGS_LTO} ${MCRECOVER_CFLAGS_CONFIG_COMMON}")
SET(CMAKE_CXX_FLAGS_RELEASE		"${MCRECOVER_CXXFLAGS_COMPILER_RELEASE} -DNDEBUG ${MCRECOVER_CFLAGS_LTO} ${MCRECOVER_CXXFLAGS_CONFIG_COMMON}")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG	"${MCRECOVER_LDFLAGS_COMPILER_DEBUG} ${MCRECOVER_LDFLAGS_CONFIG_COMMON}")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE	"${MCRECOVER_LDFLAGS_COMPILER_RELEASE} ${MCRECOVER_LDFLAGS_CONFIG_COMMON} ${MCRECOVER_LDFLAGS_LTO}")

# Unset the temporary variables.
UNSET(MCRECOVER_C99_CFLAG)
UNSET(MCRECOVER_CXX11_COMPAT_HEADER)
UNSET(MCRECOVER_CXX11_CXXFLAG)
UNSET(MCRECOVER_CFLAGS_COMMON)
UNSET(MCRECOVER_CFLAGS_PLATFORM)
UNSET(MCRECOVER_LDFLAGS_COMMON)
UNSET(MCRECOVER_LDFLAGS_PLATFORM)
UNSET(MCRECOVER_CFLAGS_CONFIG_COMMON)
UNSET(MCRECOVER_CXXFLAGS_CONFIG_COMMON)
