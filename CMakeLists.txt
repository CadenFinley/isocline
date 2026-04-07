cmake_minimum_required(VERSION 3.16)

project(isocline LANGUAGES C)

include(CTest)

add_subdirectory(cjsh-isocline)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
