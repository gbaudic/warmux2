SET(EXECUTABLE warmux)

FILE(GLOB_RECURSE WARMUX_SRCS *.cpp *.c)
IF(NOT WIN32 AND NOT MSYS)
        LIST(REMOVE_ITEM WARMUX_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/msvc/config.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/msvc/getopt.c
                                     ${CMAKE_CURRENT_SOURCE_DIR}/msvc/getopt.h
        )
ENDIF()
# Exclude maemo files as is already the case in autotools
LIST(FILTER WARMUX_SRCS EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/maemo/.*)
SET(WARMUX_SRCS ${WARMUX_SRCS} PARENT_SCOPE)

INCLUDE_DIRECTORIES(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/lib/warmux/include
        ${PROJECT_SOURCE_DIR}/lib/fixedpoint
)

ADD_EXECUTABLE(${EXECUTABLE} ${WARMUX_SRCS})
SET_TARGET_PROPERTIES(${EXECUTABLE} PROPERTIES LINK_FLAGS "${WARMUX_LDFLAGS}" COMPILE_FLAGS "${WARMUX_CFLAGS}")

TARGET_LINK_LIBRARIES( ${EXECUTABLE}
        libwarmux
        SDL2::Image SDL2::TTF SDL2::Net SDL2::Mixer SDL2::GFX
        LibXml2::LibXml2
        CURL::libcurl
        PNG::PNG
        SDL2::Core
        Intl::Intl
        libfixedpoint
)

IF (FRIBIDI_FOUND)
TARGET_LINK_LIBRARIES( ${EXECUTABLE}
        ${FRIBIDI_LIBRARY}
)
ENDIF ()

INSTALL(TARGETS ${EXECUTABLE} RUNTIME DESTINATION ${BIN_DESTINATION})

IF(WIN32)
  # These .dll files have to be copied to src/ directory
  INSTALL(FILES
    iconv.dll
    intl.dll
    jpeg.dll
    libcurl-4.dll
    libfreetype-6.dll
    libglib-2.0-0.dll
    libglibmm-2.4-1.dll
    libgmodule-2.0-0.dll
    libgobject-2.0-0.dll
    libpng13.dll
    libsigc-2.0-0.dll
    libxml2.dll
    ogg.dll
    vorbis.dll
    vorbisfile.dll
    SDL2.dll
    SDL2_gfx.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_net.dll
    SDL2_ttf.dll
    zlib1.dll
    DESTINATION ${BIN_DESTINATION}
  )
ENDIF()
