FILE(GLOB PO_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.po)

SET(COPYRIGHT_HOLDER "Warmux Development Team")
SET(POT_FILE ${CMAKE_CURRENT_BINARY_DIR}/warmux.pot)

# This is no longer provided by FindGettext.cmake, but we need it
FIND_PROGRAM(GETTEXT_XGETTEXT_EXECUTABLE xgettext)

# Make all file paths relative to ${PROJECT_SOURCE_DIR} to keep the code pointing comments clean (#:)
SET(XGETTEXT_SOURCES)

FOREACH(FILE IN LISTS WARMUX_SRCS)
    FILE(RELATIVE_PATH REL_FILE "${PROJECT_SOURCE_DIR}" "${FILE}")
    LIST(APPEND XGETTEXT_SOURCES "${REL_FILE}")
ENDFOREACH()

# Create PO Template file 'warmux.pot'
ADD_CUSTOM_COMMAND(
  OUTPUT ${POT_FILE}
  COMMAND ${GETTEXT_XGETTEXT_EXECUTABLE}
    ARGS -o ${POT_FILE} -s --omit-header
        -c --keyword=_ --keyword=N_
       --directory=${PROJECT_SOURCE_DIR}
       --copyright-holder=${COPYRIGHT_HOLDER} ${XGETTEXT_SOURCES}
  DEPENDS ${WARMUX_SRCS}
  VERBATIM
)

# Update .po files and compile them to binary .gmo files
GETTEXT_CREATE_TRANSLATIONS(${POT_FILE} ALL ${PO_FILES})
