cmake_minimum_required(VERSION 3.0.0)
project(ass1 VERSION 0.1.0)

include(CTest)
enable_testing()

file(GLOB SOURCES
    *.h
    *.cpp
)

add_executable(ass1 ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
