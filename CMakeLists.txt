cmake_minimum_required(VERSION 3.16)

project(isocline LANGUAGES C)

include(CTest)

find_package(Python3 QUIET COMPONENTS Interpreter)
if(Python3_Interpreter_FOUND)
    add_custom_target(
        isocline_amalgamate
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/amalgamate.py"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Regenerating the standalone isocline.h"
        VERBATIM
    )
endif()

add_subdirectory(cjsh-isocline)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
