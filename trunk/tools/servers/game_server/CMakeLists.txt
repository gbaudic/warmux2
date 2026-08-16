FILE(GLOB_RECURSE WARMUX_HEADLESS_SERVER_SRCS *.cpp)
SET(EXE warmux-headless-server)

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/lib/warmux/include
                     ${PROJECT_SOURCE_DIR}/lib/warmux-server/include
                     ${PROJECT_SOURCE_DIR}/src
                     ${CMAKE_CURRENT_SOURCE_DIR} )
LINK_DIRECTORIES( ${PROJECT_SOURCE_DIR}/lib/warmux
                  ${PROJECT_SOURCE_DIR}/lib/warmux-server )

ADD_EXECUTABLE(${EXE} ${WARMUX_HEADLESS_SERVER_SRCS})
SET_TARGET_PROPERTIES(${EXE} PROPERTIES LINK_FLAGS "${WARMUX_LDFLAGS}"
                                        COMPILE_FLAGS "${WARMUX_CFLAGS}")

TARGET_LINK_LIBRARIES(${EXE} CURL::libcurl libwarmux warmux-server SDL2::Net 
    MbedTLS::mbedtls)
IF (FRIBIDI_FOUND)
  TARGET_LINK_LIBRARIES( ${EXE}
    FriBiDi::FriBiDi
  )
ENDIF ()

INSTALL(TARGETS ${EXE} RUNTIME DESTINATION ${BIN_DESTINATION})
