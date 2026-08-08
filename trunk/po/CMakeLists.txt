FILE(GLOB PO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.po)

SET(COPYRIGHT_HOLDER "Warmux Development Team")
SET(POT_FILE ${CMAKE_CURRENT_BINARY_DIR}/warmux.pot)

# This is no longer provided by FindGettext.cmake, but we need it
FIND_PROGRAM(GETTEXT_XGETTEXT_EXECUTABLE xgettext)

# Create PO Template file 'warmux.pot'
IF(APPLE)
    ADD_CUSTOM_COMMAND(
      OUTPUT ${POT_FILE}
      COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE}
        ARGS -o ${POT_FILE} -s --omit-header
            -c --keyword=_ --keyword=N_
           --copyright-holder=${COPYRIGHT_HOLDER} ${WARMUX_SRCS}
      # strip ${PROJECT_SOURCE_DIR} from the code pointing comments (#:)
    )
ELSE()
    ADD_CUSTOM_COMMAND(
      OUTPUT ${POT_FILE}
      COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE}
        ARGS -o ${POT_FILE} -s --omit-header
            -c --keyword=_ --keyword=N_
           --copyright-holder=${COPYRIGHT_HOLDER} ${WARMUX_SRCS}
      # strip ${PROJECT_SOURCE_DIR} from the code pointing comments (#:)
      COMMAND sed
        ARGS -i "s,${PROJECT_SOURCE_DIR}/*,,g" ${POT_FILE}
      DEPENDS ${WARMUX_SRCS}
    )
ENDIF()

# Update .po files and compile them to binary .gmo files
GETTEXT_CREATE_TRANSLATIONS(${POT_FILE} ALL ${PO_FILES})
