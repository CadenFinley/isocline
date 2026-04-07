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
