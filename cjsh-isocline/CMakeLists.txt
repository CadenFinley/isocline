set(
    CJSH_ISOCLINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/attr.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/bbcode.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/common.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/completers.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/completions.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editline.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/highlight.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/history.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline_env.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline_keybindings.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline_options.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline_print.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline_readline.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline_terminal.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/prompt_line_replacement.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/stringbuf.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/term.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tty.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/tty_esc.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/undo.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/unicode.c"
)

set(CJSH_ISOCLINE_COMPAT_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/include")
file(MAKE_DIRECTORY "${CJSH_ISOCLINE_COMPAT_INCLUDE_DIR}/isocline")
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/isocline.h"
    "${CJSH_ISOCLINE_COMPAT_INCLUDE_DIR}/isocline/isocline.h"
    COPYONLY
)

find_package(Threads REQUIRED)

add_library(cjsh_isocline STATIC ${CJSH_ISOCLINE_SOURCES})
add_library(isocline::isocline ALIAS cjsh_isocline)

target_include_directories(
    cjsh_isocline
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
        "$<BUILD_INTERFACE:${CJSH_ISOCLINE_COMPAT_INCLUDE_DIR}>"
)
target_link_libraries(cjsh_isocline PUBLIC Threads::Threads)
target_compile_definitions(cjsh_isocline PUBLIC IC_SEPARATE_OBJS=1)
set_target_properties(
    cjsh_isocline
    PROPERTIES
        OUTPUT_NAME isocline
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
        POSITION_INDEPENDENT_CODE ON
)
