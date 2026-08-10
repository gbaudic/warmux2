cmake_minimum_required(VERSION 3.20)

# Project name
PROJECT(Warmux)
SET(VERSION svn)

# change the default install prefix to /usr
SET(CMAKE_INSTALL_PREFIX /usr)

# Build options to be on par with autotools
option (HAVE_TWITTER "Enable twitter" OFF)
option (HAVE_FACEBOOK "Enable facebook" OFF)
option (ENABLE_SERVERS "Enable servers" ON)
option (ENABLE_FRIBIDI "Enable bidirectional text rendering" ON)

# Some aliases of cmake variables
IF(BUILD)
  SET(BUILD CACHE STRING "Abbreviation for CMAKE_BUILD_TYPE.")
  SET(CMAKE_BUILD_TYPE ${BUILD})
ENDIF()
IF(PREFIX)
  SET(PREFIX CACHE PATH "Abbreviation for CMAKE_INSTALL_PREFIX.")
  SET(CMAKE_INSTALL_PREFIX ${PREFIX})
ENDIF()

# Set some global variables
SET(WARMUX_CFLAGS "") # additional compile flags for warmux
SET(WARMUX_LDFLAGS "")		# additional linking flags for warmux
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build/cmake ${PROJECT_SOURCE_DIR}/build/cmake/sdl2)

IF(BIN_PATH)
  # If user specified a path for binary, use it
  SET(BIN_DESTINATION ${BIN_PATH})
ELSE()
  # Otherwise use defaults
  IF(UNIX AND NOT WIN32)
    SET(BIN_DESTINATION "bin")
  ELSE()
    SET(BIN_DESTINATION ${CMAKE_BINARY_DIR})
  ENDIF()
ENDIF()

IF(DATA_PATH)
  # If user specified a path for data, use it

  SET(INSTALL_DATADIR ${DATA_PATH}/data/)
  SET(INSTALL_LOCALEDIR ${DATA_PATH}/locale/)
  SET(DATA_DESTINATION ${DATA_PATH}/data)
  SET(PO_DESTINATION ${DATA_PATH}/locale)
  SET(FONT_FILE ${DATA_PATH}/data/font/Ubuntu-R.ttf)

ELSE()

  # Otherwise use defaults
  IF(UNIX AND NOT WIN32)
    SET(INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/share/warmux/)
    SET(INSTALL_LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale/)
    SET(DATA_DESTINATION "share/warmux")
    SET(PO_DESTINATION "share/locale")
    SET(FONT_FILE ${INSTALL_DATADIR}/font/Ubuntu-R.ttf)
  ELSE()
    SET(INSTALL_DATADIR data/)
    SET(INSTALL_LOCALEDIR locale/)
    SET(DATA_DESTINATION "data")
    SET(PO_DESTINATION "locale")
    SET(FONT_FILE data/font/Ubuntu-R.ttf)
  ENDIF()

ENDIF()

# Optional package CPack
INCLUDE(CPackConfig OPTIONAL)
INCLUDE(XmlValidityTest OPTIONAL)

# Use "make run" and "make debug" to run/debug warmux
ADD_CUSTOM_TARGET(run WARMUX_DATADIR=${PROJECT_SOURCE_DIR}/data/ WARMUX_FONT_PATH=${PROJECT_SOURCE_DIR}/data/font/Ubuntu-R.ttf src/warmux DEPENDS src/warmux)
ADD_CUSTOM_TARGET(debug WARMUX_DATADIR=${PROJECT_SOURCE_DIR}/data/ WARMUX_FONT_PATH=${PROJECT_SOURCE_DIR}/data/font/Ubuntu-R.ttf gdb src/warmux DEPENDS src/warmux)


# Required libraries
FIND_PACKAGE(SDL2 REQUIRED)
FIND_PACKAGE(SDL2_image REQUIRED)
FIND_PACKAGE(SDL2_mixer REQUIRED)
FIND_PACKAGE(SDL2_ttf REQUIRED)
FIND_PACKAGE(SDL2_gfx REQUIRED)
FIND_PACKAGE(SDL2_net REQUIRED)
FIND_PACKAGE(LibXml2 REQUIRED)
IF(ENABLE_FRIBIDI)
  FIND_PACKAGE(FriBiDi)
ENDIF()
FIND_PACKAGE(CURL REQUIRED)

ADD_DEFINITIONS(-DHAVE_LIBCURL)

IF(APPLE)
    # to ease packaging, we'll use a static libintl
    # we also need to link against the SDL_main library (required by the mac version of SDL)
    SET(WARMUX_LDFLAGS "-L${PROJECT_SOURCE_DIR} -lintl -liconv -framework Carbon -lSDLmain_UB ${WARMUX_LDFLAGS}")
    # to ease packaging, we'll use a framework version of libpng
    # (with a different name so there is no confusion with the UNIX version of libpng)
    SET(WARMUX_LDFLAGS "-framework wmxlibpng -I/Library/Frameworks/wmxlibpng.framework/Headers/ ${WARMUX_LDFLAGS}")
    SET(GETTEXT_FOUND TRUE)
    ADD_DEFINITIONS(-DENABLE_NLS)
    #FIND_LIBRARY(GETTEXT_LIBRARY NAMES intl
    #             PATHS ${PROJECT_SOURCE_DIR} )

    FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)
    FIND_PROGRAM(GETTEXT_XGETTEXT_EXECUTABLE xgettext)
    FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

    # additional macro
    MACRO(GETTEXT_CREATE_TRANSLATIONS _potFile _firstPoFile)

       SET(_gmoFiles)
       GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
       GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

       SET(_addToAll)
       IF(${_firstPoFile} STREQUAL "ALL")
          SET(_addToAll "ALL")
          SET(_firstPoFile)
       ENDIF()

       FOREACH (_currentPoFile ${ARGN})
          GET_FILENAME_COMPONENT(_absFile ${_currentPoFile} ABSOLUTE)
          GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
          GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
          SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

          ADD_CUSTOM_COMMAND(
             OUTPUT ${_gmoFile}
             COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
             COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
             DEPENDS ${_absPotFile} ${_absFile}
          )

          INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo)
          SET(_gmoFiles ${_gmoFiles} ${_gmoFile})

       ENDFOREACH (_currentPoFile )

       ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles})

    ENDMACRO(GETTEXT_CREATE_TRANSLATIONS )

ELSE()
  FIND_PACKAGE(PNG REQUIRED)
  FIND_PACKAGE(Gettext REQUIRED)
  FIND_PACKAGE(Intl REQUIRED)
  ADD_DEFINITIONS(-DENABLE_NLS)
ENDIF()

ADD_DEFINITIONS(${LIBXML2_DEFINITIONS})
IF(ENABLE_FRIBIDI AND FRIBIDI_FOUND)
  ADD_DEFINITIONS(-DUSE_FRIBIDI)
ENDIF()

# config.h and compile-flags
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/src/config.h.cmake ${PROJECT_SOURCE_DIR}/lib/warmux/include/config_autotools.h)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

ADD_DEFINITIONS(-DINSTALL_DATADIR="${INSTALL_DATADIR}")
ADD_DEFINITIONS(-DINSTALL_LOCALEDIR="${INSTALL_LOCALEDIR}")
ADD_DEFINITIONS(-DFONT_FILE="${FONT_FILE}")
ADD_DEFINITIONS(-DPACKAGE_VERSION="${VERSION}")

IF(MSVC)
  ADD_DEFINITIONS(-D_USE_MATH_DEFINES)
ENDIF()

IF(HAVE_FACEBOOK)
  ADD_DEFINITIONS(-DHAVE_FACEBOOK)
ENDIF()
IF(HAVE_TWITTER)
  ADD_DEFINITIONS(-DHAVE_TWITTER)
ENDIF()

IF(NOT WIN32)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-Wextra" COMPILER_SUPPORTS_WEXTRA)
  IF(COMPILER_SUPPORTS_WEXTRA)
    SET(WARMUX_CFLAGS "-Wextra ${WARMUX_CFLAGS}")
  ENDIF()

  IF(CMAKE_BUILD_TYPE MATCHES "Debug")
    SET(WARMUX_CFLAGS " ${WARMUX_CFLAGS}")
  ENDIF()

ENDIF()
IF(CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  ADD_DEFINITIONS(-DDEBUG)
  ADD_DEFINITIONS(-DWMX_LOG)
  IF(PROFILE)
    SET(WARMUX_CFLAGS "-pg ${WARMUX_CFLAGS}")
    SET(WARMUX_LDFLAGS "-pg ${WARMUX_LDFLAGS}")
  ENDIF()
ELSE()
  # optimise nondebug builds
  SET(WARMUX_CFLAGS "-O2 ${WARMUX_CFLAGS}")
  IF(NOT WIN32 AND NOT APPLE)
    SET(WARMUX_CFLAGS "-march=native ${WARMUX_CFLAGS}") # native only works on x86 and x86_64
  ENDIF()
ENDIF()


# Subdirectories
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(po)
IF(ENABLE_SERVERS)
  ADD_SUBDIRECTORY(tools/servers)
ENDIF()


# install data/
INSTALL(DIRECTORY data/ DESTINATION ${DATA_DESTINATION}
  FILE_PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ
  DIRECTORY_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ
                        GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  PATTERN "Makefile*" EXCLUDE
  PATTERN "warmux.desktop" EXCLUDE
  PATTERN "*.dtd" EXCLUDE
)

# Install desktop file and icon
IF(UNIX AND NOT APPLE)
  INSTALL(FILES data/warmux.desktop DESTINATION share/applications)
  INSTALL(FILES data/icon/warmux_128x128.png DESTINATION share/pixmaps)
ENDIF()

# Install docs
INSTALL(DIRECTORY doc DESTINATION share/
  PATTERN "Makefile*" EXCLUDE
  PATTERN "warmux.man" EXCLUDE
)
INSTALL(FILES README README.cmake INSTALL AUTHORS BUGS DESTINATION share/doc)
IF(UNIX)
  INSTALL(FILES doc/warmux.man DESTINATION share/man/man6 RENAME warmux.6)
ENDIF()

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/build/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
