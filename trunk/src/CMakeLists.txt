SET(EXECUTABLE warmux)

FILE(GLOB_RECURSE WARMUX_SRCS *.cpp *.c)
IF(NOT MSVC)
        LIST(REMOVE_ITEM WARMUX_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/msvc/getopt.c
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
        SDL2::Main
        Intl::Intl
        MbedTLS::mbedtls
        libfixedpoint
)

IF(ENABLE_FRIBIDI AND FriBiDi_FOUND)
  TARGET_LINK_LIBRARIES( ${EXECUTABLE}
        FriBiDi::FriBiDi
)
ENDIF()

IF(WIN32)
  TARGET_LINK_LIBRARIES( ${EXECUTABLE} Ws2_32)
ENDIF()

INSTALL(TARGETS ${EXECUTABLE} RUNTIME DESTINATION ${BIN_DESTINATION})

IF(WIN32)
  # These .dll files have to be copied to src/ directory
  INSTALL(FILES
    freetype.dll
    fribidi-0.dll
    iconv-2.dll
    intl-8.dll
    jpeg.dll
    libcurl-4.dll
    libfreetype-6.dll
    libpng16.dll
    libxml2.dll
    ogg.dll
    SDL2.dll
    SDL2_gfx.dll
    SDL2_image.dll
    SDL2_mixer.dll
    SDL2_net.dll
    SDL2_ttf.dll
    vorbis.dll
    vorbisfile.dll
    zlib1.dll
    DESTINATION ${BIN_DESTINATION}
  )
ENDIF()
