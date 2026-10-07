add_executable(isocline_example example.c)
target_link_libraries(isocline_example PRIVATE cjsh_isocline)

add_executable(isocline_test_colors test_colors.c)
target_link_libraries(isocline_test_colors PRIVATE cjsh_isocline)

set_target_properties(
    isocline_example
    isocline_test_colors
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
)

add_test(NAME isocline_colors COMMAND isocline_test_colors)

# Build from an isolated copy: the single header must need no library sources,
# internal include directories, or prebuilt isocline library.
set(ISOCLINE_SINGLE_HEADER_TEST_DIR "${CMAKE_CURRENT_BINARY_DIR}/single-header")
file(MAKE_DIRECTORY "${ISOCLINE_SINGLE_HEADER_TEST_DIR}")
configure_file(
    "${PROJECT_SOURCE_DIR}/isocline.h"
    "${ISOCLINE_SINGLE_HEADER_TEST_DIR}/isocline.h"
    COPYONLY
)
foreach(single_header_file IN ITEMS single_header_impl.c single_header_test.c)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${single_header_file}"
        "${ISOCLINE_SINGLE_HEADER_TEST_DIR}/${single_header_file}"
        COPYONLY
    )
endforeach()

find_package(Threads REQUIRED)
add_executable(
    isocline_test_single_header
    "${ISOCLINE_SINGLE_HEADER_TEST_DIR}/single_header_impl.c"
    "${ISOCLINE_SINGLE_HEADER_TEST_DIR}/single_header_test.c"
)
target_link_libraries(isocline_test_single_header PRIVATE Threads::Threads)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(isocline_test_single_header PRIVATE -Wall -Wextra -Werror)
endif()
set_target_properties(
    isocline_test_single_header
    PROPERTIES
        C_STANDARD 11
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
)
add_test(
    NAME isocline_single_header
    COMMAND isocline_test_single_header "${CMAKE_CURRENT_SOURCE_DIR}/single_header_input.txt"
)

if(Python3_Interpreter_FOUND)
    add_test(
        NAME isocline_single_header_current
        COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/tools/amalgamate.py" --check
    )
endif()
