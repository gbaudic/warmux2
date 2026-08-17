# - Find the native FriBiDI includes and library
#
# This module defines
#  FRIBIDI_INCLUDE_DIR, where to find fribidi.h, etc.
#  FRIBIDI_LIBRARIES, the libraries to link against to use FriBiDi.
#  FriBiDi_FOUND, If false, do not try to use fribidi.
# also defined, but not for general use are
#  FRIBIDI_LIBRARY, where to find the FriBiDi library.

include(CheckFunctionExists)
SET(FriBiDi_FOUND "NO")

FIND_PATH(FRIBIDI_INCLUDE_DIR fribidi/fribidi.h
  /usr/local/include
  /usr/include
  )

SET(FRIBIDI_NAMES ${FRIBIDI_NAMES} fribidi libfribidi)
FIND_LIBRARY(FRIBIDI_LIBRARY
  NAMES ${FRIBIDI_NAMES}
  PATHS /usr/lib /usr/local/lib
  )

IF (FRIBIDI_LIBRARY AND FRIBIDI_INCLUDE_DIR)
  SET(CMAKE_REQUIRED_INCLUDES ${FRIBIDI_INCLUDE_DIR})
  SET(CMAKE_REQUIRED_LIBRARIES ${FRIBIDI_LIBRARY})
  CHECK_FUNCTION_EXISTS(fribidi_log2vis FOUND_fribidi_log2vis)
  IF(FOUND_fribidi_log2vis)
    SET(FRIBIDI_LIBRARIES ${FRIBIDI_LIBRARY})
    SET(FriBiDi_FOUND "YES")
  ELSE()
    SET(FRIBIDI_LIBRARIES "NOTFOUND")
    SET(FRIBIDI_INCLUDE_DIR "NOTFOUND")
    SET(FriBiDi_FOUND "NO")
  ENDIF()
ENDIF ()

IF (FriBiDi_FOUND)
  IF (NOT FRIBIDI_FIND_QUIETLY)
    MESSAGE(STATUS "Found FriBiDi: ${FRIBIDI_LIBRARY}")
  ENDIF ()
  # FriBiDi::FriBiDi target
  IF (FRIBIDI_LIBRARY AND NOT TARGET FriBiDi::FriBiDi)
    ADD_LIBRARY(FriBiDi::FriBiDi UNKNOWN IMPORTED)
    SET_TARGET_PROPERTIES(FriBiDi::FriBiDi PROPERTIES
                          IMPORTED_LOCATION "${FRIBIDI_LIBRARY}"
                          INTERFACE_INCLUDE_DIRECTORIES "${FRIBIDI_INCLUDE_DIR}")
  ENDIF()
ELSE ()
  IF (FRIBIDI_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find FriBiDi library")
  ENDIF ()
ENDIF ()
